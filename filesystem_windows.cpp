
struct File_Windows {
  HANDLE handle;
};
static_assert(sizeof(File) >= sizeof(File_Windows), "");

static void convert_ascii_bytes_to_windows_string(const char* str, wchar_t* win, size_t length) {
    for (size_t i = 0; i < length; i++) {
        win[i] = str[i];
    }
    win[length] = L'\0';
}

bool file_open_windows(File* file, const char* filename) {
  File_Windows* f = (File_Windows*) file;

  f->handle = CreateFileA(filename,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL,
                          NULL);

  if (!f->handle || f->handle == INVALID_HANDLE_VALUE) {
    return false; // @LogError: 
  }

  // @LogError: check if ERROR_FILE_NOT_FOUND.

  return true;
}

bool file_close_windows(File* file) {
  File_Windows* f = (File_Windows*) file;

  // @LogError: 
  return f->handle ? CloseHandle(f->handle) : true;
}

bool file_read_windows(File* file, void* buffer, size_t to_read, size_t* written) {
  File_Windows* f = (File_Windows*) file;

  DWORD bytes_written;
  BOOL success = ReadFile(f->handle, buffer, (DWORD)to_read, &bytes_written, NULL); // @ErrorProne: when we are going to read file larger than DWORD can address we are gonna crash right here. @PassUIntsNotSizeT: @What?!
  *written = bytes_written;

  if (success) {
    return true;
  } else {
    return false;  // @LogError: 
  }
}

bool file_write_windows(File* file, const char* buffer, size_t to_write) {
  File_Windows* f = (File_Windows*) file;

  BOOL success = WriteFile(f->handle, buffer, (DWORD)to_write, NULL, NULL);

  if (success) {
    return true;
  } else {
    return false; // @LogError: 
  }
}

size_t file_get_size_windows(File* file) {
  File_Windows* f = (File_Windows*) file;

  LARGE_INTEGER file_size;
  if(GetFileSizeEx(f->handle, &file_size)) {
    return file_size.QuadPart;
  } else {
    return 0;
  }
}

void check_filesystem_api() {
    assert(file_open);
    assert(file_close);
    assert(file_read);
    assert(file_write);
    assert(file_get_size);
}

void init_filesystem_api() {
  file_open     = file_open_windows;
  file_close    = file_close_windows;
  file_read     = file_read_windows;
  file_write    = file_write_windows;
  file_get_size = file_get_size_windows;
}
