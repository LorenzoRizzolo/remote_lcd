import { env } from "$env/dynamic/private";
import fs from "fs";

export async function GET({ url }) {
	const token = url.searchParams.get("token");
	const isValid = token === env.API_TOKEN;

	if (isValid) {
		const message = fs.readFileSync(".message", "utf-8");
		// Return a proper Response
		return new Response(message, {
			status: 200,
			headers: { "Content-Type": "text/plain" }
		});
	}

	return new Response("UNAUTHORIZED", { status: 401 });
}
