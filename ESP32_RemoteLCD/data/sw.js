self.addEventListener('install', (event) => {
  event.waitUntil((async () => {
    const cache = await caches.open('remote-lcd-v1');
    await cache.addAll([
      '/',
      '/index.html',
      '/manifest.json'
    ]);
    self.skipWaiting();
  })());
});

self.addEventListener('activate', (event) => {
  event.waitUntil(self.clients.claim());
});

self.addEventListener('fetch', (event) => {
  const { request } = event;
  if (request.method !== 'GET') return;
  event.respondWith((async () => {
    const cache = await caches.open('remote-lcd-v1');
    const cached = await cache.match(request);
    if (cached) return cached;
    try {
      const response = await fetch(request);
      if (response && response.ok && response.type === 'basic') {
        cache.put(request, response.clone());
      }
      return response;
    } catch (e) {
      return cached || Response.error();
    }
  })());
});