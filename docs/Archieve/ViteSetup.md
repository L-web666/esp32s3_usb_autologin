https://vite.dev/guide/
https://developer.mozilla.org/en-US/docs/Web/JavaScript

npm create vite@latest site2
cd site2
npm install
npm run dev
npm run build

Here's a complete setup for a **Vite project** that supports multiple HTML pages (e.g., `index.html`, `about.html`) and follows modern best practices. This setup ensures a clean and scalable project structure.

---

### 1. **Project Structure**

The recommended structure for a multi-page Vite project:

```
project-root/
├── public/
│   ├── index.html         # Home page
│   ├── about.html         # About page
│   ├── css/
│   │   └── styles.css     # Shared styles
│   ├── images/
│   │   └── favicon.ico    # Static assets
├── src/
│   ├── index.js           # JavaScript for index.html
│   ├── about.js           # JavaScript for about.html
│   ├── shared/
│   │   └── utils.js       # Shared JavaScript modules
├── vite.config.js         # Vite configuration
├── package.json           # Project dependencies
├── .gitignore             # Ignored files
└── README.md              # Project documentation
```

---

### 2. **Setup Instructions**

#### Step 1: Initialize a New Vite Project
Test with vanilla and javascript
```bash
npm create vite@latest my-multipage-app
cd my-multipage-app
npm install
```

#### Step 2: Install Dependencies
Ensure the necessary dependencies are installed:
```bash
npm install vite-plugin-html --save-dev
```

#### Step 3: Configure Vite for Multi-Page Support
Update `vite.config.js` to include multiple entry points for each page:

**vite.config.js**
```javascript
import { defineConfig } from 'vite';
import { createHtmlPlugin } from 'vite-plugin-html';
import path from 'path';

export default defineConfig({
  build: {
    rollupOptions: {
      input: {
        main: path.resolve(__dirname, 'public/index.html'),
        about: path.resolve(__dirname, 'public/about.html'),
      },
    },
    outDir: 'dist', // Output directory
    emptyOutDir: true,
  },
  plugins: [
    createHtmlPlugin({
      inject: {
        data: {
          title: 'My Multi-Page App',
        },
      },
    }),
  ],
});
```

---

#### Step 4: Create the `public` Directory and HTML Files

**public/index.html**
```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Home</title>
  <link rel="stylesheet" href="/css/styles.css">
  <script type="module" src="/src/index.js"></script>
</head>
<body>
  <h1>Welcome to the Home Page</h1>
  <a href="/about.html">Go to About Page</a>
</body>
</html>
```

**public/about.html**
```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>About</title>
  <link rel="stylesheet" href="/css/styles.css">
  <script type="module" src="/src/about.js"></script>
</head>
<body>
  <h1>About Us</h1>
  <a href="/index.html">Back to Home</a>
</body>
</html>
```

---

#### Step 5: Add JavaScript Files for Each Page

**src/index.js**
```javascript
console.log('Welcome to the Home Page!');
```

**src/about.js**
```javascript
console.log('Welcome to the About Page!');
```

---

#### Step 6: Add a Shared Stylesheet

**public/css/styles.css**
```css
body {
  font-family: Arial, sans-serif;
  margin: 0;
  padding: 20px;
}

h1 {
  color: #333;
}

a {
  color: #007bff;
  text-decoration: none;
}

a:hover {
  text-decoration: underline;
}
```

---

#### Step 7: Build and Test the Project

1. **Run Development Server**:
   ```bash
   npm run dev
   ```

   Access the pages:
   - [Home Page](http://localhost:5173/index.html)
   - [About Page](http://localhost:5173/about.html)

2. **Build the Project**:
   ```bash
   npm run build
   ```

   After building, check the `dist` folder:
   ```
   dist/
   ├── index.html
   ├── about.html
   ├── assets/
   │   ├── index.<hash>.js
   │   ├── about.<hash>.js
   │   ├── styles.<hash>.css
   ```

3. **Preview Production Build**:
   ```bash
   npm run preview
   ```

---

### Summary

This setup provides:
1. **Separation of Concerns**: HTML in `public/`, JavaScript in `src/`, and styles in `public/css`.
2. **Multi-Page Support**: Multiple entry points (`index.html` and `about.html`) configured via `rollupOptions`.
3. **Best Practices**: Static assets served directly from `public/` and Vite processing for source files in `src/`.

You can extend this structure for more pages (e.g., `contact.html`) by repeating the pattern. 🎉