import { json } from "@sveltejs/kit";
import fs from "fs"

export async function POST({ request }) {
	const { token } = await request.json();

	const isValid = fs.existsSync(`./sessions/${token}`)
	if(isValid){
		return json({ valid: isValid, text:fs.readFileSync(".message").toString() });
	}
	return json({ valid: isValid });
}
