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

	// Предварительно вычисленные суммы для каждого сообщения
	expectedSums := []int{903, 966, 961}

	conn, err := net.Dial("tcp", address)
	if err != nil {
		fmt.Println("Ошибка подключения:", err)
		return
	}
	defer conn.Close()

	start := time.Now()
	errors := 0 // Счетчик ошибок

	for i := 0; i < 50000; i++ {
		msgIndex := rand.Intn(len(messages))
		data := messages[msgIndex]
		expectedSum := expectedSums[msgIndex]

		// Отправка данных
		_, err = conn.Write(data)
		if err != nil {
			fmt.Println("Ошибка отправки данных:", err)
			return
		}

		// Получение ответа
		buffer := make([]byte, 12)
		n, err := conn.Read(buffer)
		if err != nil {
			fmt.Println("Ошибка чтения данных:", err)
			return
		}

		// Парсинг ответа сервера
		response := string(buffer[:n])
		receivedSum, err := strconv.Atoi(response)
		if err != nil {
			fmt.Printf("Ошибка преобразования ответа '%s': %v\n", response, err)
			errors++
			continue
		}

		// Проверка корректности
		if receivedSum != expectedSum {
			fmt.Printf("Ошибка! Ожидали %d, получили %d\n", expectedSum, receivedSum)
			errors++
		}
	}

	elapsed := time.Since(start)
	fmt.Printf("\nТест завершен!\n")
	fmt.Printf("Время выполнения: %d мс\n", elapsed.Milliseconds())
	// fmt.Printf("Ошибок: %d/500\n", errors)
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
	fmt.Printf("Суммарное время выполнения: %d мс\n", elapsed.Milliseconds())
}
