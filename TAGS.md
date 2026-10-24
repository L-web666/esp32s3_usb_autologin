# Tag History

This document tracks the history of tags for the project, including the tag name, date, and a brief description of the changes or features included in each tag.

To add a new tag, run the following commands in your terminal:

```bash
git tag -a v1.0.X -m "Description of the tag"
git push origin v1.0.X
```

Or during testing delete tags like this:

```bash
git tag -d v1.0.X # Delete the tag locally
git push origin --delete v1.0.X # Delete the tag from the remote
```
## Tags

### Tag: v1.0.0
**Date:** 2025-24-03
**Description:**  
- Copy of original

### Tag: v1.0.1
**Date:** 2025-24-03
**Description:**  
- Cleanup actions

### Tag: v1.0.2
**Date:** 2025-25-03
**Description:**  
- Cleanup categories

### Tag: v1.0.3
**Date:** 2025-26-03
**Description:**
- Add encrypted passwords

### Tag: v1.0.4
**Date:** 2025-26-03
**Description:**
- Mqtt cleanup
- Add exponential back off

### Tag: v1.0.5
**Date:** 2025-26-03
**Description:**
- Add button commands

### Tag: v1.0.6
**Date:** 2025-27-03
**Description:**
- Add clear preferences

### Tag: v1.0.7
**Date:** 2025-28-03
**Description:**
- Cleanup add password form
- Cleanup DuckyScript handler
- Add ability to type out files
- Add samples

### Tag: v1.0.8
**Date:** 2025-02-04
**Description:**
- Upgrade espressif32@6.10.0
- Implemented DeviceConfig settings from NVM
- Cleanup settings heap stack
- Optimize CryptoHandler
- Unstable need to revert to (v1.0.7)

### Tag: v1.0.9
**Date:** 2025-02-04
**Description:**
- Revert to v1.0.7
- Implemented PrintFile

### Tag: v1.0.10
**Date:** 2025-??-04
**Description:**
- Upgrade espressif32@6.10.0
-