package main

import (
    "crypto/sha256"
    "encoding/binary"
    "fmt"

    "api"
)

// send nullifier, SHA256(0x00 || rho)
func computeSendNullifier(rho []byte) []byte {
	h := sha256.New()
	h.Write([]byte{0x00})
	h.Write(rho)
	return h.Sum(nil)
}

func computeSpendNullifier(rho []byte, sk [32]byte) []byte {
    h := sha256.New()
    h.Write([]byte{0x01})
    h.Write(rho)
    h.Write(sk[:])
    return h.Sum(nil)
}

// cm = SHA256(rho || pk || v) where v is in little endian byte order
func computeCommitment(rho []byte, pk [32]byte, v uint64) []byte {
	vbuf := make([]byte, 8)
	binary.LittleEndian.PutUint64(vbuf, v)

	h := sha256.New()
	h.Write(rho[:])
	h.Write(pk[:])
	h.Write(vbuf)

	return h.Sum(nil)
}

func main() {

    //rho := []byte{0xa5,0x6a,0xd5,0x28,0xdd,0x5f,0x2a,0xac,0x45,0x24,0x4c,0xfc,0xba,0x8f,0x88,0x35,0x45,0x22,0x67,0x19,0x39,0xa0,0xf2,0x73,0xcd,0xbf,0x50,0xf6,0x2d,0xab,0x85,0x1c}
    //sk := [32]byte{0xa5,0x6a,0xd5,0x28,0xdd,0x5f,0x2a,0xac,0x45,0x24,0x4c,0xfc,0xba,0x8f,0x88,0x35,0x45,0x22,0x67,0x19,0x39,0xa0,0xf2,0x73,0xcd,0xbf,0x50,0xf6,0x2d,0xab,0x85,0x1c}
    //spend_nf: 0x4a11ee7d155524c148a51d8d4e3c5a9230a0492dc689c1c832276c0c1301961b
    
    /*sk := [32]byte{0xf0,0xf0,0xf0,0xf0,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xff,0xf0,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x00,0xf0,0x00,0x0f,0x0f,0x00,0xf0,0x0f,0x0f,0x0f,0x0f,0x00,0xff}
    rho := []byte{0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd,0xde,0xde,0xff,0xdd} 
    pk := [32]byte{0xe8,0xe5,0x5f,0x61,0x7b,0x4b,0x69,0x30,0x83,0xf8,0x83,0xf7,0x09,0x26,0xdd,0x56,0x73,0xfa,0x43,0x4c,0xef,0xa3,0x66,0x08,0x28,0x75,0x99,0x47,0xe2,0x27,0x63,0x48};
    
    
    fmt.Printf("rho: %x\n", rho)
    fmt.Printf("pk: %x\n", pk)
    fmt.Printf("sk: %x\n", sk)
    send_nf := computeSendNullifier(rho)
    fmt.Printf("send_nf: %x\n", send_nf)

    spend_nf := computeSpendNullifier(rho, sk)
    fmt.Printf("spend_nf: %x\n", spend_nf)
    
    var value uint64 = 2378237
    cm := computeCommitment(rho, pk, value)
    fmt.Printf("commitment: %x\n", cm)
    */
    
    var a = []byte{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    var b = []byte{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
    
    h := sha256.New()
    h.Write(a)
    h.Write(b)
    fmt.Printf("combine zero: %x\n", h.Sum(nil)) 
}