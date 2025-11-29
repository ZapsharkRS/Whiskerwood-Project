#include "WhiskerwoodFileIOLibrary.h"

#if WITH_EDITOR

#include "HAL/PlatformFileManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Normalize path and ensure it contains "Whiskerwood" somewhere (case-insensitive).
// Used for any DESTINATION path for writes/deletes. 
static bool IsWhiskerwoodSafeDestPath(const FString& InPath, FString& OutNormalizedPath)
{
    OutNormalizedPath = InPath;

    // Normalize slashes and case
    // Example: C:/Users/.../Whiskerwood/... -> c:/users/.../whiskerwood/...
    FPaths::MakeStandardFilename(OutNormalizedPath);

    const FString SafetyToken = TEXT("whiskerwood");

    if (!OutNormalizedPath.Contains(SafetyToken, ESearchCase::IgnoreCase, ESearchDir::FromStart))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: Refusing to operate on destination path '%s' because it does not contain '%s'."),
            *OutNormalizedPath,
            *SafetyToken
        );
        return false;
    }

    if (OutNormalizedPath.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: Refusing to operate on an empty destination path."));
        return false;
    }

    return true;
}

// For delete operations we use the same safety rules as other destructive writes.
// If you ever want different rules, you can split this into a dedicated helper.
static bool IsWhiskerwoodSafeDeletePath(const FString& InPath, FString& OutNormalizedPath)
{
    return IsWhiskerwoodSafeDestPath(InPath, OutNormalizedPath);
}

// ---------------------------------------------------------------------------
// Delete operations
// ---------------------------------------------------------------------------

bool UWhiskerwoodFileIOLibrary::DeleteWhiskerwoodFile(const FString& FilePath)
{
    FString NormalizedPath;
    if (!IsWhiskerwoodSafeDeletePath(FilePath, NormalizedPath))
    {
        // Failed safety check
        return false;
    }

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.FileExists(*NormalizedPath))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: File does not exist, cannot delete. Path: '%s'"),
            *NormalizedPath
        );
        return false;
    }

    const bool bDeleted = PlatformFile.DeleteFile(*NormalizedPath);

    if (!bDeleted)
    {
        UE_LOG(LogTemp, Error,
            TEXT("WhiskerwoodFileIOLibrary: Failed to delete file: '%s'"),
            *NormalizedPath
        );
    }
    else
    {
        UE_LOG(LogTemp, Log,
            TEXT("WhiskerwoodFileIOLibrary: Successfully deleted file: '%s'"),
            *NormalizedPath
        );
    }

    return bDeleted;
}

bool UWhiskerwoodFileIOLibrary::DeleteWhiskerwoodDirectory(const FString& DirectoryPath)
{
    FString NormalizedPath;
    if (!IsWhiskerwoodSafeDeletePath(DirectoryPath, NormalizedPath))
    {
        // Failed safety check
        return false;
    }

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!PlatformFile.DirectoryExists(*NormalizedPath))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: Directory does not exist, cannot delete. Path: '%s'"),
            *NormalizedPath
        );
        return false;
    }

    const bool bDeleted = PlatformFile.DeleteDirectoryRecursively(*NormalizedPath);

    if (!bDeleted)
    {
        UE_LOG(LogTemp, Error,
            TEXT("WhiskerwoodFileIOLibrary: Failed to recursively delete directory: '%s'"),
            *NormalizedPath
        );
    }
    else
    {
        UE_LOG(LogTemp, Log,
            TEXT("WhiskerwoodFileIOLibrary: Successfully recursively deleted directory: '%s'"),
            *NormalizedPath
        );
    }

    return bDeleted;
}

// ---------------------------------------------------------------------------
// Directory creation
// ---------------------------------------------------------------------------

bool UWhiskerwoodFileIOLibrary::EnsureWhiskerwoodDirectory(const FString& DirectoryPath)
{
    FString NormalizedPath;
    if (!IsWhiskerwoodSafeDestPath(DirectoryPath, NormalizedPath))
    {
        return false;
    }

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (PlatformFile.DirectoryExists(*NormalizedPath))
    {
        UE_LOG(LogTemp, Verbose,
            TEXT("WhiskerwoodFileIOLibrary: Directory already exists: '%s'"),
            *NormalizedPath
        );
        return true;
    }

    const bool bCreated = PlatformFile.CreateDirectoryTree(*NormalizedPath);

    if (!bCreated)
    {
        UE_LOG(LogTemp, Error,
            TEXT("WhiskerwoodFileIOLibrary: Failed to create directory tree: '%s'"),
            *NormalizedPath
        );
    }
    else
    {
        UE_LOG(LogTemp, Log,
            TEXT("WhiskerwoodFileIOLibrary: Successfully created directory tree: '%s'"),
            *NormalizedPath
        );
    }

    return bCreated;
}

// ---------------------------------------------------------------------------
/* Copy file (source → Whiskerwood destination) */
// ---------------------------------------------------------------------------

bool UWhiskerwoodFileIOLibrary::CopyWhiskerwoodFile(const FString& SourceFilePath, const FString& DestFilePath, bool bOverwriteExisting)
{
    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    // Normalize source; we allow it to be anywhere, but ensure it's not empty.
    FString NormalizedSource = SourceFilePath;
    FPaths::MakeStandardFilename(NormalizedSource);

    if (NormalizedSource.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: Source file path is empty, cannot copy."));
        return false;
    }

    if (!PlatformFile.FileExists(*NormalizedSource))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: Source file does not exist, cannot copy. Path: '%s'"),
            *NormalizedSource
        );
        return false;
    }

    // Check safety & normalize destination.
    FString NormalizedDest;
    if (!IsWhiskerwoodSafeDestPath(DestFilePath, NormalizedDest))
    {
        return false;
    }

    // If overwrite is not allowed and dest exists, bail.
    if (!bOverwriteExisting && PlatformFile.FileExists(*NormalizedDest))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: Destination file already exists and overwrite is disabled. Dest: '%s'"),
            *NormalizedDest
        );
        return false;
    }

    // Ensure destination directory exists.
    const FString DestDir = FPaths::GetPath(NormalizedDest);
    if (!DestDir.IsEmpty())
    {
        FString NormalizedDestDir = DestDir;
        if (!EnsureWhiskerwoodDirectory(NormalizedDestDir))
        {
            UE_LOG(LogTemp, Error,
                TEXT("WhiskerwoodFileIOLibrary: Failed to ensure destination directory for copy. DestDir: '%s'"),
                *NormalizedDestDir
            );
            return false;
        }
    }

    // Unreal's CopyFile just uses source + dest; overwrite is controlled
    // by whether you allow an existing file to remain in place.
    const bool bCopied = PlatformFile.CopyFile(*NormalizedDest, *NormalizedSource);

    if (!bCopied)
    {
        UE_LOG(LogTemp, Error,
            TEXT("WhiskerwoodFileIOLibrary: Failed to copy file from '%s' to '%s'"),
            *NormalizedSource,
            *NormalizedDest
        );
    }
    else
    {
        UE_LOG(LogTemp, Log,
            TEXT("WhiskerwoodFileIOLibrary: Successfully copied file from '%s' to '%s'"),
            *NormalizedSource,
            *NormalizedDest
        );
    }

    return bCopied;
}

// ---------------------------------------------------------------------------
// Write text file (e.g. simple metadata file next to .pak)
// ---------------------------------------------------------------------------

bool UWhiskerwoodFileIOLibrary::WriteWhiskerwoodTextFile(const FString& FilePath, const FString& Text, bool bAllowOverwrite)
{
    FString NormalizedPath;
    if (!IsWhiskerwoodSafeDestPath(FilePath, NormalizedPath))
    {
        return false;
    }

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!bAllowOverwrite && PlatformFile.FileExists(*NormalizedPath))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("WhiskerwoodFileIOLibrary: File already exists and overwrite is disabled. Path: '%s'"),
            *NormalizedPath
        );
        return false;
    }

    // Ensure directory exists
    const FString Dir = FPaths::GetPath(NormalizedPath);
    if (!Dir.IsEmpty())
    {
        FString NormalizedDir = Dir;
        if (!EnsureWhiskerwoodDirectory(NormalizedDir))
        {
            UE_LOG(LogTemp, Error,
                TEXT("WhiskerwoodFileIOLibrary: Failed to ensure directory for write. Dir: '%s'"),
                *NormalizedDir
            );
            return false;
        }
    }

    const bool bSaved = FFileHelper::SaveStringToFile(
        Text,
        *NormalizedPath,
        FFileHelper::EEncodingOptions::AutoDetect
        // Using default FileManager and no special flags
    );

    if (!bSaved)
    {
        UE_LOG(LogTemp, Error,
            TEXT("WhiskerwoodFileIOLibrary: Failed to write text file: '%s'"),
            *NormalizedPath
        );
    }
    else
    {
        UE_LOG(LogTemp, Log,
            TEXT("WhiskerwoodFileIOLibrary: Successfully wrote text file: '%s'"),
            *NormalizedPath
        );
    }

    return bSaved;
}

#endif // WITH_EDITOR
