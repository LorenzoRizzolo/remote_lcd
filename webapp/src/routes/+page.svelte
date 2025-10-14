<script>
    import { goto } from '$app/navigation';
    import { env } from '$env/dynamic/public';

    import { onMount } from "svelte";
	import { checkToken } from "$lib/auth";

	// controlla la validità del token
	onMount(async () => {
		const valid = await checkToken();
		if (!valid) {
			localStorage.removeItem("token");
		}else{
			goto("/text");
		}
	});
    
    const API_BASE_URL = env.PUBLIC_API_URL;
    
    let password = '';
    
    let error = '';
    let loading = false;

    /**
     * @description Funzione che chiama l'API /login per autenticare l'utente.
     */
    async function handleLogin() {
        error = '';
        loading = true;

        if (!password) {
            error = "The password can't be empty"
            loading = false;
            return;
        }

        try {
            const url = `${API_BASE_URL}/login`;
            const response = await fetch(url, {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({ password }), 
            });

            const result = await response.json();

            if (response.ok) {
                localStorage.setItem("token", result.token)
                // redirect user after login
                goto('/text'); 
            } else {
                // Login Failed
                error = result.message || 'Access not permitted';
            }

        } catch (e) {
            console.error('Network or API error:', e);
            error = 'Is impossible to connect to login service';
        } finally {
            loading = false;
        }
    }
</script>

<div class="center">
    <div class="psw_box">
        <div>Insert Password</div>
        <input 
            type="password" 
            placeholder="password..." 
            bind:value={password}
            on:keydown={(e) => { 
                if (e.key === 'Enter') {
                    handleLogin();
                }
            }}
        >
        
        <button on:click={handleLogin} disabled={loading}>
            {#if loading}
                Verifing...
            {:else}
                Login
            {/if}
        </button>

        {#if error}
            <p style="color: red; margin-top: 10px;">{error}</p>
        {/if}
    </div>
</div>



<style>
    .center{
        margin-top: 50px;
    }

    .psw_box{
        width: 80%;
        max-width: 300px;
        margin: auto;

        display: flex;
        align-items: center;
        justify-content: center;
        flex-direction: column;
        padding: 15px;
    }

    input{
        border: 0;
        padding: 5px 10px;
        outline-width: 0;
        text-align: center;
        margin-top: 20px;
        margin-bottom: 10px;
        background-color: rgba(255, 255, 255, 0);

        background: rgba(255, 255, 255, 0.2);
        border-radius: 10px;
        box-shadow: 0 4px 30px rgba(0, 0, 0, 0.1);
        backdrop-filter: blur(5px);
        -webkit-backdrop-filter: blur(5px);
        border: 1px solid rgba(255, 255, 255, 0.3);
    }
</style>