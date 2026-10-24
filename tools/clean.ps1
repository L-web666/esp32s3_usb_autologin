# This script is used to clean the project by deleting the unnecessary folders
cd ..

# Set the paths to search for
$foldersToDelete = @("logs", "bin", "obj", ".vs", ".idea", ".vscode", ".pio", "dist", "node_modules")

# Function to delete folders
function Delete-Folders {
    param (
        [string[]]$folderNames
    )

    foreach ($folderName in $folderNames) {
        Write-Host "Searching and deleting '$folderName' folders..."

        # Search for directories recursively and delete them
        Get-ChildItem -Path . -Recurse -Directory -Force |
        Where-Object { $_.Name -in $folderNames } |
        ForEach-Object {
            try {
                Remove-Item -Path $_.FullName -Recurse -Force -ErrorAction Stop
                Write-Host "Deleted folder: $($_.FullName)" -ForegroundColor Green
            } catch {
                Write-Host "Failed to delete folder: $($_.FullName). Error: $_" -ForegroundColor Red
            }
        }
    }
}

# Call the function
Delete-Folders -folderNames $foldersToDelete


# -----------------------------------------------------------------------------------------
# MANUAL COMMANDS TO RUN:
#
# 1. List all directories in the current folder and subfolders (recursively):
#    - Use this command to search for the specific folders you want to delete.
#    Command:
#      Get-ChildItem -Path . -Recurse -Directory -Force
#
# 2. Search for specific folder names (e.g., 'logs', 'bin', 'obj', '.vs', '.idea'):
#    - This command filters the folders based on their names.
#    Command:
#      Get-ChildItem -Path . -Recurse -Directory -Force | Where-Object { $_.Name -in @("logs", "bin", "obj", ".vs", ".idea") }
#
# 3. Delete specific folders (e.g., 'logs', 'bin', 'obj', '.vs', '.idea'):
#    - This command will delete the folders you searched for in the previous step.
#    Command:
#      Get-ChildItem -Path . -Recurse -Directory -Force | Where-Object { $_.Name -in @("logs", "bin", "obj", ".vs", ".idea") } | ForEach-Object { Remove-Item -Path $_.FullName -Recurse -Force }
#
# 4. Handle errors while deleting folders (optional):
#    - To manually handle errors, wrap the `Remove-Item` command in a try-catch block.
#    Example:
#      try {
#          Remove-Item -Path "path_to_folder" -Recurse -Force -ErrorAction Stop
#      } catch {
#          Write-Host "Failed to delete folder: path_to_folder"
#      }
#
# -----------------------------------------------------------------------------------------
