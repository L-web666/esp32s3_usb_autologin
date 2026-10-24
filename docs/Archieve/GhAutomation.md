GitHub provides a REST API that supports accessing release information in JSON format. You can use this to automate the generation of your manifest files. Below is a detailed explanation of how you can approach this and streamline your process.

---

### Accessing Release Information via GitHub API
The GitHub REST API has an endpoint to list releases for a repository:
```
GET https://api.github.com/repos/{owner}/{repo}/releases
```
This will return JSON data for all releases, including their tag names, assets, download URLs, etc.

Example JSON snippet:
```json
[
  {
    "tag_name": "v1.0.0",
    "assets": [
      {
        "name": "test.bin",
        "browser_download_url": "https://github.com/mdelgert/ImprovWiFiWeb/releases/download/v1.0.0/test.bin"
      }
    ]
  },
  {
    "tag_name": "v1.0.1",
    "assets": [
      {
        "name": "test.bin",
        "browser_download_url": "https://github.com/mdelgert/ImprovWiFiWeb/releases/download/v1.0.1/test.bin"
      }
    ]
  }
]
```

---

### Automating Manifest File Creation

1. **Fetch Release Information**:
   Use a script or GitHub Action to fetch release data using the API.

2. **Generate Manifest Files**:
   Parse the release JSON, and for each release, generate a corresponding `.json` manifest file.

3. **Deploy Manifest Files**:
   Include these files in your GitHub Pages workflow.

---

### Example GitHub Action to Automate Manifests

```yaml
name: Generate and Deploy Manifests

on:
  workflow_run:
    workflows:
      - PlatformIO Build
    types:
      - completed

jobs:
  generate-manifests:
    runs-on: ubuntu-latest

    steps:
      - name: Checkout repository
        uses: actions/checkout@v3

      - name: Fetch Releases from GitHub API
        run: |
          curl -H "Authorization: token ${{ secrets.GITHUB_TOKEN }}" \
            https://api.github.com/repos/mdelgert/ImprovWiFiWeb/releases \
            -o releases.json

      - name: Generate Manifest Files
        run: |
          mkdir -p site/dist/firmware
          python3 <<EOF
          import json
          import os

          # Load releases data
          with open('releases.json', 'r') as f:
              releases = json.load(f)

          # Generate manifest files
          for release in releases:
              tag = release['tag_name']
              for asset in release.get('assets', []):
                  if asset['name'].endswith('.bin'):
                      manifest = {
                          "name": "Test",
                          "version": tag,
                          "home_assistant_domain": "esphome",
                          "funding_url": "https://github.com/mdelgert/ImprovWiFiWeb",
                          "new_install_prompt_erase": False,
                          "builds": [
                              {
                                  "chipFamily": "ESP32-S3",
                                  "parts": [{"path": asset['name'], "offset": 0}]
                              }
                          ]
                      }
                      with open(f"site/dist/firmware/{tag}.json", "w") as mf:
                          json.dump(manifest, mf, indent=2)
          EOF

      - name: Deploy to GitHub Pages
        uses: peaceiris/actions-gh-pages@v4
        with:
          github_token: ${{ secrets.GITHUB_TOKEN }}
          publish_dir: site/dist
```

---

### Benefits of This Approach
1. **Dynamic Updates**:
   Automatically updates manifest files whenever a new release is created.

2. **Centralized Management**:
   Leverages GitHub's API to ensure all release information is consistent.

3. **Integration with GitHub Pages**:
   Automatically deploys updated manifest files to your GitHub Pages site.

4. **Extensible**:
   You can expand the script to support additional metadata or features.

---