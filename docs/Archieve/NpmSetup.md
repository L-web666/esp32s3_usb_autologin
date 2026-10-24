Here’s what you need to do to get your project running on a new machine after pulling it from GitHub:

---

### Steps to Make the Project Run

#### 1. **Install Node.js and npm**
Ensure Node.js and npm are installed on your system. You can check their versions using:
```bash
node -v
npm -v
```
If not installed, download and install from [Node.js official site](https://nodejs.org/).

---

#### 2. **Navigate to the Project Directory**
Navigate to the directory where your project was cloned:
```bash
cd your-project-folder/website
```

---

#### 3. **Install Dependencies**
Run `npm install` to install all required dependencies specified in `package.json`:
```bash
npm install
```

---

#### 4. **Run the Development Server**
Start the development server to preview your project:
```bash
npx vite
```
The local server should start, and you can access your project at `http://localhost:3000` (or the port specified by Vite).

---

### Additional Tasks

#### **To Build for Production**
If you want to build the project for production:
```bash
npm run build
```
This will generate the `dist` folder with your production-ready files.

#### **To Preview the Production Build**
You can preview the production build with:
```bash
npx vite preview
```
This serves the contents of the `dist` folder locally.

---

### Common Issues and Fixes

1. **Missing `vite` or `npm install` fails**:
   Ensure the correct version of Node.js is installed. Refer to the `.nvmrc` file (if available) for the required version or use Node.js 18.x (as per your GitHub Actions setup).

2. **Error: `command not found: npx`**:
   Ensure npm is installed correctly. Try re-installing Node.js.

3. **Missing Environment Variables**:
   If your project relies on environment variables (e.g., `.env`), make sure to copy the required `.env` file or set the necessary variables.

4. **Permission Issues on Linux/macOS**:
   If you encounter permission issues, try using `sudo` or fix file permissions:
   ```bash
   sudo chmod -R 755 your-project-folder
   ```

---

### Summary

The primary commands you'll need are:
1. Install dependencies:
   ```bash
   npm install
   ```
2. Run the development server:
   ```bash
   npx vite
   ```
3. Build for production:
   ```bash
   npm run build
   ```

Let me know if you encounter any issues during setup! 🚀