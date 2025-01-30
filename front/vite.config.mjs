import { defineConfig } from 'vite';
import { nodeResolve } from '@rollup/plugin-node-resolve';
import tailwindcss from '@tailwindcss/vite';

export default defineConfig({
  plugins: [nodeResolve(), tailwindcss()],
  server: {
    port: 5173,
    watch: {
      ignored: ['**/_opam'],
    },
  },
  build: {
    sourcemap: true,
  }
});
