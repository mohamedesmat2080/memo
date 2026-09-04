using System;
using System.Net;
using System.Net.Mail;
using System.Threading.Tasks;

namespace JTGuard.Helpers
{
    public class EmailService
    {
        private const string SmtpServer = "smtp.gmail.com";
        private const int SmtpPort = 587;
        private const string SenderEmail = "lexashieldonline@gmail.com";
        private const string SenderPassword = "ubvuhnigaukvilxi"; // App Password for Gmail

        public async Task SendEmailAsync(string recipient, string subject, string body)
        {
            try
            {
                using (SmtpClient smtpClient = new SmtpClient(SmtpServer))
                {
                    smtpClient.Port = SmtpPort;
                    smtpClient.Credentials = new NetworkCredential(SenderEmail, SenderPassword);
                    smtpClient.EnableSsl = true;

                    using (MailMessage mailMessage = new MailMessage())
                    {
                        mailMessage.From = new MailAddress(SenderEmail);
                        mailMessage.To.Add(recipient);
                        mailMessage.Subject = subject;
                        mailMessage.Body = body;
                        mailMessage.IsBodyHtml = true; // If your body is in HTML format

                        // Asenkron olarak e-posta gönderme işlemi
                        await smtpClient.SendMailAsync(mailMessage);

                        //Console.WriteLine("Email sent successfully.");
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error sending email: {ex.Message}");
            }
        }
    }
}
