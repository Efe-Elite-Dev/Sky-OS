import tkinter as tk
from tkinter import messagebox
import subprocess
import time
import os

class SkyOOBE(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Sky-OS Kurulum")
        self.attributes('-fullscreen', True)
        self.configure(bg='white')
        self.current_step = 1
        
        self.main_frame = tk.Frame(self, bg='white')
        self.main_frame.pack(expand=True, fill='both')
        self.show_step()

    def clear_frame(self):
        for widget in self.main_frame.winfo_children():
            widget.destroy()

    def show_step(self):
        self.clear_frame()
        if self.current_step == 1: self.step_welcome()
        elif self.current_step == 2: self.step_wifi()
        elif self.current_step == 3: self.step_ready()

    def step_welcome(self):
        lbl = tk.Label(self.main_frame, text="Sky-OS'e Hoş Geldiniz", font=("Helvetica", 48, "bold"), bg='white', fg='black')
        lbl.pack(expand=True)
        self.after(3000, self.next_step)

    def step_wifi(self):
        lbl = tk.Label(self.main_frame, text="Wi-Fi Bağlantısı Kontrol Ediliyor...", font=("Helvetica", 28), bg='white', fg='black')
        lbl.pack(pady=100)
        
        def check_internet():
            try:
                result = subprocess.run(["ping", "-c", "1", "8.8.8.8"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                if result.returncode == 0:
                    self.next_step()
                else:
                    self.after(2000, check_internet)
            except:
                self.after(2000, check_internet)

        self.after(1000, check_internet)
        
        skip_btn = tk.Button(self.main_frame, text="Bağlantıyı Atla", font=("Helvetica", 14), bg='white', fg='gray', bd=0, command=self.next_step)
        skip_btn.pack(side='bottom', pady=50)

    def step_ready(self):
        lbl = tk.Label(self.main_frame, text="Sistem Hazırlanıyor, Nitro Açıldı!", font=("Helvetica", 36, "bold"), bg='white', fg='black')
        lbl.pack(expand=True)
        self.after(2500, self.finish_setup)

    def next_step(self):
        self.current_step += 1
        self.show_step()

    def finish_setup(self):
        self.destroy()
        os.system("bash /usr/local/bin/sky-init.sh")

if __name__ == "__main__":
    app = SkyOOBE()
    app.mainloop()
