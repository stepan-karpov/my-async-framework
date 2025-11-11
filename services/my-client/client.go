package main

import (
	"fmt"
	"math/rand"
	"net"
	"strconv"
	"sync"
	"time"
)

func testTCPConnection() {
	address := "localhost:8082"

	messages := [][]byte{
		{0x69, 0x20, 0x6C, 0x30, 0x76, 0x65, 0x20, 0x63, 0x61, 0x30, 0x73, 0x00}, // "i l0ve ca0s" (sum = 903)
		{0x73, 0x30, 0x63, 0x6B, 0x33, 0x74, 0x20, 0x74, 0x33, 0x73, 0x74, 0x00}, // "s0ck3t t3st" (sum = 966)
		{0x6E, 0x33, 0x74, 0x77, 0x30, 0x72, 0x6B, 0x20, 0x6D, 0x34, 0x67, 0x00}, // "n3tw0rk m4g" (sum = 961)
	}

	// Precomputed sums for each message
	expectedSums := []int{903, 966, 961}

	conn, err := net.Dial("tcp", address)
	if err != nil {
		fmt.Println("Connection error:", err)
		return
	}
	defer conn.Close()

	start := time.Now()
	errors := 0 // Error counter

	for i := 0; i < 50000; i++ {
		msgIndex := rand.Intn(len(messages))
		data := messages[msgIndex]
		expectedSum := expectedSums[msgIndex]

		// Send data
		_, err = conn.Write(data)
		if err != nil {
			fmt.Println("Error sending data:", err)
			return
		}

		// Receive response
		buffer := make([]byte, 12)
		n, err := conn.Read(buffer)
		if err != nil {
			fmt.Println("Error reading data:", err)
			return
		}

		// Parse server response
		response := string(buffer[:n])
		receivedSum, err := strconv.Atoi(response)
		if err != nil {
			fmt.Printf("Error converting response '%s': %v\n", response, err)
			errors++
			continue
		}

		// Verify correctness
		if receivedSum != expectedSum {
			fmt.Printf("Error! Expected %d, got %d\n", expectedSum, receivedSum)
			errors++
		}
	}

	elapsed := time.Since(start)
	fmt.Printf("\nTest completed!\n")
	fmt.Printf("Execution time: %d ms\n", elapsed.Milliseconds())
	// fmt.Printf("Errors: %d/500\n", errors)
}

func main() {
	var wg sync.WaitGroup

	start := time.Now()

	for i := 0; i < 15; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			testTCPConnection()
		}()
	}

	wg.Wait()

	elapsed := time.Since(start)
	fmt.Printf("Total execution time: %d ms\n", elapsed.Milliseconds())
}
