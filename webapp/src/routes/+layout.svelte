<script>
	import favicon from '$lib/assets/favicon.svg';
	import "../app.css"

	let { children } = $props();
</script>

<svelte:head>
	<link rel="icon" href={favicon} />
</svelte:head>

<header>
	<h1 class="font">RemoteLCD</h1>
</header>

{@render children?.()}
