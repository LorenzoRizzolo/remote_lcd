// src/routes/login/+server.js
import { json, redirect } from "@sveltejs/kit";
import { generateToken } from "$lib/backend/functions";
import { env } from "$env/dynamic/private";

import fs from "fs"

export async function POST({ request, cookies }) {
	const { password } = await request.json();

	if (password === env.LOGIN_PASSWORD) {

		const token = generateToken()
		if(!fs.existsSync("./sessions")){
			fs.mkdirSync("./sessions")
		}

		fs.writeFileSync(`./sessions/${token}`, "")

		return json({ success: true, token });
	}

	return new Response(
		JSON.stringify({ error: "Incorrect password" }),
		{ status: 401 }
	);
}
