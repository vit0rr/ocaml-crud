import { defineConfig } from 'vite'
import { nodeResolve } from '@rollup/plugin-node-resolve'
import tailwindcss from '@tailwindcss/vite'

export default defineConfig({
  plugins: [nodeResolve(), tailwindcss(),],
  server: {
    watch: {
      ignored: ['**/_opam']
    }
  },
});
