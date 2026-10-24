import os
import json
import requests
from pathlib import Path
import shutil
import argparse

# Constants (You can adjust these for your project)
LIMIT_RELEASES = 10  # Set to 0 for no limit (process all releases)
GITHUB_API_URL = "https://api.github.com/repos/mdelgert/PassTxt/releases"
MANIFEST_TEMPLATE = {
    # "home_assistant_domain": "esphome",
    "funding_url": "https://github.com/mdelgert/PassTxt",
    "new_install_prompt_erase": True,
}

# Function to fetch releases from GitHub API
def fetch_releases(api_url, auth_token=None):
    headers = {"Accept": "application/vnd.github.v3+json"}
    if auth_token:
        headers["Authorization"] = f"token {auth_token}"

    response = requests.get(api_url, headers=headers)
    response.raise_for_status()  # Raise an exception for HTTP errors
    return response.json()

# Function to download a binary file
def download_binary(url, output_file):
    try:
        response = requests.get(url, stream=True)
        response.raise_for_status()  # Raise an exception for HTTP errors

        with open(output_file, "wb") as f:
            for chunk in response.iter_content(chunk_size=8192):
                f.write(chunk)
        print(f"Downloaded binary: {output_file}")
    except Exception as e:
        print(f"Error downloading binary file from {url}: {e}")
        raise

# Function to generate a manifest and download binaries for each release
def generate_manifest_and_download(release, output_dir):
    tag_name = release.get("tag_name")
    assets = release.get("assets", [])

    for asset in assets:
        if asset["name"].endswith(".bin"):
            # Ensure the output directory exists
            os.makedirs(output_dir, exist_ok=True)

            # Define the binary filename with tag name
            binary_filename = f"{tag_name}.bin"
            binary_filepath = os.path.join(output_dir, binary_filename)

            # Download the binary file
            download_binary(asset["browser_download_url"], binary_filepath)

            # Create the manifest
            manifest = MANIFEST_TEMPLATE.copy()
            manifest["name"] = "Demo"
            manifest["version"] = tag_name
            manifest["builds"] = [
                {
                    "chipFamily": "ESP32-S3",
                    "parts": [{"path": binary_filename, "offset": 0}],
                }
            ]

            # Write the manifest to a JSON file
            manifest_filename = os.path.join(output_dir, f"{tag_name}.json")
            with open(manifest_filename, "w") as f:
                json.dump(manifest, f, indent=2)
            print(f"Generated manifest: {manifest_filename}")

# Main script
def main():
    # Parse arguments
    parser = argparse.ArgumentParser(description="Generate manifests and download binaries.")
    parser.add_argument(
        "--output-dir",
        type=str,
        default="site/dist/firmware",
        help="The output directory for the manifests and binaries",
    )
    args = parser.parse_args()
    output_dir = args.output_dir

    # Environment variable for GitHub token (optional for private repos)
    auth_token = os.getenv("GITHUB_TOKEN")

    try:
        # Fetch releases from GitHub
        releases = fetch_releases(GITHUB_API_URL, auth_token)

        # Apply the limit if it is greater than 0
        if LIMIT_RELEASES > 0:
            releases = releases[:LIMIT_RELEASES]

        # Generate manifests and download binaries for each release
        for release in releases:
            generate_manifest_and_download(release, output_dir)

        print("All manifests and binaries processed successfully.")
    except requests.RequestException as e:
        print(f"Error fetching releases: {e}")
    except FileNotFoundError as e:
        print(f"File operation error: {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    main()
