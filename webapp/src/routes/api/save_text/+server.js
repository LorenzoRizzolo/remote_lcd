import { json } from "@sveltejs/kit";
import fs from "fs"

export async function POST({ request }) {
	const { token, text } = await request.json();

	const isValid = fs.existsSync(`./sessions/${token}`)
	if(isValid){
		fs.writeFileSync(".message", text)
		return json({ ok:true })
	}

	return new Response(
		JSON.stringify({ error: "Incorrect token" }),
		{ status: 401 }
	);
}
