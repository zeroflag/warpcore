#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "image.h"
#include "vm.h"

const char DUMP_NAME[] = "dump.img";

Ver read_version(uint8_t *image) {
  Ver ver;
  ver.major = image[0] >> 4;
  ver.minor = image[0] & 15;
  return ver;
}

int fd_size(int fd) {
  struct stat fst;
  return fstat(fd, &fst) == -1 ? -1 : fst.st_size;
}

char *fd_map(int fd, int size) {
  return mmap(NULL, size,
              PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

uint8_t *map_file(const char* path) {
  int fd = open(path, O_RDWR);
  if (fd != -1) {
    long size = fd_size(fd);
    if (size != MEM_SIZE) {
      close(fd);
      breach("Image size must be: %d got: %d\n", MEM_SIZE, size);
    }
    uint8_t *data = (uint8_t*) fd_map(fd, size);
    if (data == MAP_FAILED) {
      close(fd);
      breach("Cannot mmap image: %s\n", path);
    }
    close(fd);
    return data;
  } else {
    breach("Could not open image: %s\n", path);
    return NULL;
  }
}

void load_file(const char *path, uint8_t* buffer) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
      breach("Cannot open image: %s\n", path);
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
      fclose(fp);
      breach("Cannot get size of image: %s\n", path);
    }
    if (ftell(fp) != MEM_SIZE) {
      fclose(fp);
      breach("Image size must be: %d\n", MEM_SIZE);
    }
    rewind(fp);
    size_t read = fread(buffer, 1, MEM_SIZE, fp);
    fclose(fp);
    if (read != (size_t)MEM_SIZE) {
      fprintf(stderr, "Short read: expected %d, got %zu\n", MEM_SIZE, read);
      free(buffer);
    }
}

void sync_mapped_image(uint8_t* mem) {
  msync(mem, MEM_SIZE, MS_SYNC);
}

int dump(const uint8_t* mem) {
  FILE *fp = fopen(DUMP_NAME, "wb");
  if (!fp) {
    breach("Cannot create dump file: %s", DUMP_NAME);
    return 0;
  }
  size_t written = fwrite(mem, 1, MEM_SIZE, fp);
  if (written != (size_t)MEM_SIZE) {
    fprintf(stderr, "Short write: expected %d, wrote %zu\n", MEM_SIZE, written);
    fclose(fp);
    return 0;
  }
  fclose(fp);
  return -1;
}
