import fs from "fs"

export async function logout(token){
    fs.unlinkSync(`./sessions/${token}`)
}