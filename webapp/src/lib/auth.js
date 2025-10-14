import { env } from "$env/dynamic/public";

export async function checkToken() {
	const token = localStorage.getItem("token");
	if (!token) return false;

	try {
		const res = await fetch(`${env.PUBLIC_API_URL}/validate`, {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ token })
		});
		const data = await res.json();
		return data.valid;
	} catch {
		return false;
	}
}

export async function logout() {
	const token = localStorage.getItem("token");
	if (!token) return false;

	try {
		const res = await fetch(`${env.PUBLIC_API_URL}/logout`, {
			method: "POST",
			headers: { "Content-Type": "application/json" },
			body: JSON.stringify({ token })
		});
		const data = await res.json();
		if(data.logout){
			localStorage.removeItem("token")
		}
		return data.logout;
	} catch {
		return false;
	}
}
