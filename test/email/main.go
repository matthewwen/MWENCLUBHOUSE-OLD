package main

import (
	"bytes"
	"log"
	"net/smtp"
)

func main() {
	// Connect to the remote SMTP server.
	c, err := smtp.Dial("mail.example.com:25")
	if err != nil {
		log.Fatal(err)
	}
	defer c.Close()
	// Set the sender and recipient.
	c.Mail("sender@example.org")
	c.Rcpt("mattwen2018@gmail.com")
	// Send the email body.
	wc, err := c.Data()
	if err != nil {
		log.Fatal(err)
	}
	defer wc.Close()
	buf := bytes.NewBufferString("This is the email body.")
	if _, err = buf.WriteTo(wc); err != nil {
		log.Fatal(err)
	}
}
