To revert your `main` branch back to the stable `v1.0.7` tag in Git, you can follow these steps. This assumes you have the repository cloned locally and have appropriate permissions to modify the `main` branch. Here's how you can do it:

1. **Switch to the `main` branch** (if you're not already on it):
   ```bash
   git checkout main
   ```

2. **Reset the `main` branch to the `v1.0.7` tag**:
   Use the `git reset` command to move the `main` branch back to the commit associated with `v1.0.7`. The `--hard` option will discard all changes after that tag:
   ```bash
   git reset --hard v1.0.7
   ```

3. **Push the changes to the remote repository**:
   Since this rewrites history, you'll need to force-push the update to the remote repository (e.g., GitHub). Be cautious, as this will overwrite the remote `main` branch:
   ```bash
   git push origin main --force
   ```

### Important Notes:
- **Collaboration Warning**: If others are working on the same repository, force-pushing will overwrite their history. Make sure to communicate with your team before doing this.
- **Backup (Optional)**: If you want to preserve the unstable `v1.0.8` changes somewhere, create a new branch before resetting:
   ```bash
   git checkout main
   git branch backup-v1.0.8
   ```
   Then proceed with the reset.

### Verification:
After resetting, you can confirm that `main` is now at `v1.0.7` by checking the log:
```bash
git log
```
Look for the commit hash or message tied to `v1.0.7`.