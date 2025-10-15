export async function handle({ resolve, event }) {
  const origin = event.request.headers.get('origin');

  // Risposta alle OPTIONS per preflight CORS
  if (event.url.pathname.startsWith('/api') && event.request.method === 'OPTIONS') {
    return new Response(null, {
      status: 204, // No Content
      headers: {
        'Access-Control-Allow-Methods': 'GET, POST, PUT, DELETE, PATCH, OPTIONS',
        'Access-Control-Allow-Origin': origin || '', // origin preciso
        'Access-Control-Allow-Headers': event.request.headers.get('access-control-request-headers') || '',
        'Access-Control-Allow-Credentials': 'true',
      },
    });
  }

  // Risposta per tutte le altre richieste API
  const response = await resolve(event);

  if (event.url.pathname.startsWith('/api')) {
    // ATTENZIONE: non usare append ma set (in caso di header già esistente)
    response.headers.set('Access-Control-Allow-Origin', origin || '');
    response.headers.set('Access-Control-Allow-Credentials', 'true');
  }

  return response;
}
