#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WhiskerwoodFileIOLibrary.generated.h"

/**
 * File/directory IO helpers that are hard-limited
 * so that any write/delete destination must contain "Whiskerwood".
 *
 * Intended for Editor Utility Widgets & other editor tools that manage
 * Whiskerwood mod .pak files and metadata.
 *
 * NOTE: Class is always compiled so packaging (UnrealGame target) works.
 * Actual destructive behavior is guarded in the .cpp with WITH_EDITOR.
 */
UCLASS()
class WHISKERWOODMODTOOLS_API UWhiskerwoodFileIOLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    /**
     * Returns the Whiskerwood mods directory under the current user's Local AppData.
     *
     * Example (Windows):
     *   C:/Users/<User>/AppData/Local/Whiskerwood/Saved/mods
     */
    UFUNCTION(BlueprintPure, Category="Whiskerwood|ModUtils|File IO", meta=(DevelopmentOnly))
    static FString GetWhiskerwoodModsDirectory();

    /**
     * Deletes a file on disk, but ONLY if the normalized path contains "Whiskerwood".
     */
    UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|File IO", meta=(DevelopmentOnly))
    static bool DeleteWhiskerwoodFile(const FString& FilePath);

    /**
     * Recursively deletes a directory on disk, but ONLY if the normalized path contains "Whiskerwood".
     */
    UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|File IO", meta=(DevelopmentOnly))
    static bool DeleteWhiskerwoodDirectory(const FString& DirectoryPath);

    /**
     * Ensures that a directory (and parents) exists.
     * Only succeeds if the normalized path contains "Whiskerwood".
     */
    UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|File IO", meta=(DevelopmentOnly))
    static bool EnsureWhiskerwoodDirectory(const FString& DirectoryPath);

    /**
     * Copies a file from SourceFilePath to DestFilePath.
     * Source can be anywhere, but DestFilePath MUST contain "Whiskerwood".
     *
     * @param SourceFilePath Full path to the file to copy from.
     * @param DestFilePath Full path to the file to copy to (must contain "Whiskerwood").
     * @param bOverwriteExisting If true, will overwrite an existing file at destination.
     */
    UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|File IO", meta=(DevelopmentOnly))
    static bool CopyWhiskerwoodFile(const FString& SourceFilePath, const FString& DestFilePath, bool bOverwriteExisting);

    /**
     * Writes simple text to a file in a Whiskerwood path (e.g. metadata next to the .pak).
     * Will auto-create the directory tree if needed.
     *
     * @param FilePath Full path of the file to write to (must contain "Whiskerwood").
     * @param Text Text content to write.
     * @param bAllowOverwrite If false and the file already exists, this will fail.
     */
    UFUNCTION(BlueprintCallable, Category="Whiskerwood|ModUtils|File IO", meta=(DevelopmentOnly))
    static bool WriteWhiskerwoodTextFile(const FString& FilePath, const FString& Text, bool bAllowOverwrite);
};
