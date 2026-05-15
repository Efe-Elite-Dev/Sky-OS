import google.generativeai as genai
import os
import sys

# Gemini API Bağlantısı (İleride kendi API keyini buraya gömeceksin aga)
genai.configure(api_key="SKY_OS_AI_KEY")

def execute_system_command(user_voice_input):
    system_instruction = """
    Sen Sky-OS işletim sisteminin çekirdeğindeki yapay zeka beynisin. 
    Kullanıcı sana sesle komut verdiğinde, sadece ve sadece terminalde çalıştırılacak olan Linux komutunu döndür. 
    Ekstra hiçbir açıklama, yazı veya markdown işareti ekleme.
    
    Örnekler:
    - 'Discord yükle' -> sudo snap install discord
    - 'Sistemi kapat' -> shutdown now
    - 'Spotify kur' -> sudo snap install spotify
    """
    
    try:
        model = genai.GenerativeModel("gemini-3-flash", system_instruction=system_instruction)
        response = model.generate_content(user_voice_input)
        
        system_command = response.text.strip()
        print(f"[Sky-OS] Çalıştırılan Komut: {system_command}")
        
        # Komutu doğrudan root terminalinde ateşle!
        os.system(system_command)
    except Exception as e:
        print(f"Hata oluştu kaptan: {e}")

if __name__ == "__main__":
    if len(sys.argv) > 1:
        user_text = " ".join(sys.argv[1:])
        execute_system_command(user_text)
