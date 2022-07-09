
struct File {
  unsigned char blob[8];
};

bool   (*file_open)(File*, const char*);
bool   (*file_close)(File*);
bool   (*file_read)(File*, void*, size_t, size_t*);
bool   (*file_write)(File*, const char*, size_t);
size_t (*file_get_size)(File*); // @Incomplete: this could fail too, so we should return boolean.
void check_filesystem_api();
void init_filesystem_api();


literal read_entire_file(literal filename) {
  literal result = {};

  HANDLE handle = CreateFileA(filename.data, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
  if(handle != INVALID_HANDLE_VALUE) {
    LARGE_INTEGER file_size;
    if(GetFileSizeEx(handle, &file_size)) {
      size_t size = file_size.QuadPart;
      if(void* data = alloc(size+1)) {
        DWORD bytes_read;
        if(ReadFile(handle, data, size, &bytes_read, 0) && size == bytes_read) {
          ((char*)data)[size] = '\0';
          result.data  = (const char*) data;
          result.count = size;
        } else {
          print("ReadFile failed :)\n");
        }
      } else {
        print("VirtualAlloc failed :)\n");
      }
    } else {
      print("GetFileSizeEx failed :)\n");
    }

    CloseHandle(handle);
  } else {
    print("CreateFileA failed :)\n");
  }

  return result;
}


int write_to_file(literal filename, literal string) {
  HANDLE handle = CreateFileA(filename.data, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
  if(handle != INVALID_HANDLE_VALUE) {
    DWORD bytes_written;
    if(WriteFile(handle, string.data, string.count, &bytes_written, 0)) {
      return bytes_written == string.count;
    } else {
      print("WriteFile failed :)\n");
    }

    CloseHandle(handle);
  } else {
    print("CreateFileA failed :)\n");
  }

  return false;
}
