import adapter from '@sveltejs/adapter-node'; 

const config = {
  kit: {
    // Altre configurazioni del kit...
    adapter: adapter({
      // Specifica la directory di output
      outDir: 'build'
    })
  }
};
export default config;
