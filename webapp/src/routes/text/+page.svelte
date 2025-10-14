<script>
    import { goto } from "$app/navigation";
    import { onMount } from "svelte";
    import { checkToken, logout } from "$lib/auth";
    import { env } from "$env/dynamic/public";

    let mode = "list"
    let text = ""
    let message = ""
    let loading = false;

    onMount(async () => {
        const token = localStorage.getItem("token")
		const valid = await checkToken();
		if (!valid) {
			localStorage.removeItem("token");
            goto("/")
		}
        const url = `${env.PUBLIC_API_URL}/get_text`;
        const response = await fetch(url, {
            method: 'POST',
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({ token }), 
        });
        const result = await response.json();
        if(response.ok){ text=result.text }
	});

    async function save_text() {
        message = '';
        loading = true;
        const token = localStorage.getItem("token")

        try {
            const url = `${env.PUBLIC_API_URL}/save_text`;
            const response = await fetch(url, {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({ token, text }), 
            });

            const result = await response.json();

            if (response.ok) {
                message = "Text saved correctly"
            } else {
                message = "Error"
            }

        } catch (e) {
            console.error('Network or API error:', e);
            message = 'Is impossible to connect to server service';
        } finally {
            loading = false;
        }
    }

</script>

<h1 class="font center">
    Insert Text to Send to LCD Screen
</h1>


{#if mode=="list"}
    <div class="options">
        <button class="opt" on:click={()=>{mode="insert"}}>
            Insert Text
        </button>
        <button class="opt" on:click={()=>{
            logout()
            goto("/")
        }}>
            Logout
        </button>
    </div>
{:else if mode=="insert"}
    <div class="options">
        {#if message}
            <p>{message}</p>
        {/if}
        <div>
            <button on:click={()=>{mode="list"}}>Back</button>
            <button on:click={save_text}>Save</button>
        </div>
        <br>
        <textarea bind:value={text} placeholder="Insert text for LCD screen..."></textarea>
    </div>
{/if}


<style>
    .options{
        display: flex;
        align-items: center;
        justify-content: center;
        flex-direction: column;
    }

    .opt{
        flex: 1;
        width: 80%;
        padding: 15px;
        margin-top: 10px;
        text-align: center;

        background: rgba(255, 255, 255, 0.2);
        border-radius: 10px;
        box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
        backdrop-filter: blur(5px);
        -webkit-backdrop-filter: blur(5px);
        border: 1px solid rgba(255, 255, 255, 0.3);

        font-family: "Iceberg", sans-serif;
    }

    textarea{
        width: 80%;

        background: rgba(255, 255, 255, 0.2);
        border-radius: 10px;
        box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
        backdrop-filter: blur(5px);
        -webkit-backdrop-filter: blur(5px);
        border: 1px solid rgba(255, 255, 255, 0.3);
        padding: 10px;
        outline-width: 0;

        resize: none;
        height: 200px;
    }
</style>