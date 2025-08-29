import os
import chardet

def convert_file_to_utf8(filename):
    # Определяем кодировку файла
    with open(filename, 'rb') as f:
        raw_data = f.read()
        result = chardet.detect(raw_data)
        encoding = result['encoding']

    # Если кодировка win-1251, конвертируем в utf-8
    if encoding == 'windows-1251':
        with open(filename, 'r', encoding='windows-1251') as f:
            content = f.read()
        
        with open(filename, 'w', encoding='utf-8') as f:
            f.write(content)
        print(f"Файл {filename} успешно конвертирован в UTF-8.")
    else:
        print(f"Файл {filename} имеет кодировку {encoding}, конвертация не требуется.")

def main():
    # Получаем список всех файлов в текущей директории
    files = [f for f in os.listdir() if os.path.isfile(f)]

    # Обрабатываем каждый файл
    for file in files:
        convert_file_to_utf8(file)

if __name__ == "__main__":
    main()