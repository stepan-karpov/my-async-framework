package main

import (
	"fmt"
	"net"
	"time"
)

func main() {
	address := "localhost:8082"

	data := []byte{0x69, 0x20, 0x6C, 0x30, 0x76, 0x65, 0x20, 0x63, 0x61, 0x30, 0x73, 0x00}

	conn, err := net.Dial("tcp", address)
	if err != nil {
		fmt.Println("Ошибка подключения:", err)
		return
	}
	defer conn.Close()

	_, err = conn.Write(data)
	if err != nil {
		fmt.Println("Ошибка отправки данных:", err)
		return
	}
	fmt.Println("Данные отправлены успешно")

	// Начинаем измерение времени
	start := time.Now()

	// Читаем ответ от сервера
	buffer := make([]byte, 1024)
	n, err := conn.Read(buffer)
	if err != nil {
		fmt.Println("Ошибка чтения данных:", err)
		return
	}

	// Вычисляем время ответа
	elapsed := time.Since(start)

	fmt.Println("Получено от сервера:", string(buffer[:n]))
	fmt.Printf("Время ответа: %d мкрс\n", elapsed.Nanoseconds())
}
