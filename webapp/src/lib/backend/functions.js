import crypto from "crypto";

export function generateToken() {
  return crypto.randomBytes(16).toString("hex"); // 16 bytes = 32 caratteri esadecimali
}
