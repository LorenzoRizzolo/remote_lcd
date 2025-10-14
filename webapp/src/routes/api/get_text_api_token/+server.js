import { env } from "$env/dynamic/private";
import { json } from "@sveltejs/kit";
import fs from "fs";

export async function GET({ url }) {
	// Ottieni il token dalla query string
	const token = url.searchParams.get("token");

	const isValid = token === env.API_TOKEN;

	if (isValid) {
		const message = fs.readFileSync(".message", "utf-8");
		return json({ valid: true, text: message });
	}

	return json({ valid: false });
}
