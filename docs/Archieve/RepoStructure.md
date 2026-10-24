
---

### **Step 1: Plan Folder Structure**
We need a structure that accommodates both:
1. A website that uses GitHub Pages for hosting.
2. A PlatformIO project for building the firmware.

Proposed structure:
```
/<repository-name>
├── .github/                     <- GitHub workflows for automation
│   └── workflows/
│       └── deploy.yml           <- GitHub Actions for auto-deploying the website and firmware
├── platformio/                  <- PlatformIO project folder
│   ├── src/
│   ├── include/
│   ├── lib/
│   ├── platformio.ini           <- PlatformIO configuration file
├── website/                     <- Website files for GitHub Pages
│   ├── public/
│   │   ├── index.html           <- Website entry point
│   │   └── manifest.json        <- ESP Web Tools manifest (later)
│   ├── src/
│   │   └── app.js               <- JavaScript for the website (later)
│   ├── vite.config.js           <- Vite configuration file
│   └── package.json             <- Node.js dependencies for the website
├── README.md                    <- Documentation
└── LICENSE                      <- Project license
```

---

### **Step 2: Set Up the GitHub Repository**
1. Create a new repository on GitHub with a suitable name (e.g., `esp-web-tools-demo`).
2. Clone the repository locally:
   ```bash
   git clone https://github.com/<your-username>/<repository-name>.git
   cd <repository-name>
   ```

---

### **Step 3: Create the Website**
1. Initialize the website folder:
   ```bash
   mkdir website
   cd website
   npm init -y
   ```

2. Install Vite:
   ```bash
   npm install vite --save-dev
   ```

3. Set up a simple Vite project:
   - Create `website/public/index.html`:
     ```html
     <!DOCTYPE html>
     <html lang="en">
     <head>
       <meta charset="UTF-8">
       <meta name="viewport" content="width=device-width, initial-scale=1.0">
       <title>ESP Web Tools Demo</title>
     </head>
     <body>
       <h1>Hello, World!</h1>
     </body>
     </html>
     ```

   - Create `website/vite.config.js`:
     ```javascript
     import { defineConfig } from 'vite';

     export default defineConfig({
       root: 'public',
       build: {
         outDir: '../dist', // Output directory for GitHub Pages
       },
     });
     ```

4. Test the website locally:
   ```bash
   npx vite
   ```

   Visit `http://localhost:5173/` to verify that the "Hello, World!" page loads.

5. Commit the initial website:
   ```bash
   git add website
   git commit -m "Add initial website with Vite setup"
   ```

---

### **Step 4: Enable GitHub Pages**
1. In your repository on GitHub:
   - Go to **Settings → Pages**.
   - Set the source to **GitHub Actions**.

---

### **Step 5: Add GitHub Actions for Website Deployment**
1. Create a workflow for GitHub Pages deployment:
   - Create `.github/workflows/deploy.yml`:
     ```yaml
     name: Deploy Website

     on:
       push:
         branches:
           - main

     jobs:
       build-deploy:
         runs-on: ubuntu-latest
         steps:
           - name: Checkout repository
             uses: actions/checkout@v3

           - name: Set up Node.js
             uses: actions/setup-node@v3
             with:
               node-version: 18

           - name: Install dependencies
             working-directory: website
             run: npm install

           - name: Build website
             working-directory: website
             run: npm run build

           - name: Deploy to GitHub Pages
             uses: peaceiris/actions-gh-pages@v3
             with:
               github_token: ${{ secrets.GITHUB_TOKEN }}
               publish_dir: website/dist
     ```

2. Push the workflow:
   ```bash
   git add .github
   git commit -m "Add GitHub Actions for website deployment"
   git push origin main
   ```

---

### **Step 6: Verify the Deployment**
1. After the workflow completes, your site will be live at:
   - `https://<your-username>.github.io/<repository-name>/`

2. Visit the URL to ensure the "Hello, World!" page is displayed.

---

### **Next Steps**
Once the website is live, we can:
1. Add the `esp-web-tools` integration to the website.
2. Set up the `platformio/` folder with a basic firmware project.
3. Extend the GitHub Actions workflow to:
   - Build the PlatformIO firmware.
   - Attach the firmware binary to GitHub Releases for easy access.