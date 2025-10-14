<script>
	import "../app.css"

	let { children } = $props();
</script>

<svelte:head>
	<title>RemoteLCD</title>
</svelte:head>

<header>
	<h1 class="font">RemoteLCD</h1>
</header>

{@render children?.()}
