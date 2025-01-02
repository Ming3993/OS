// Khai báo hàm walk
extern pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);

#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl
  uint64 va, ua;
  int pnum;

  // Lấy tham số
  if (argaddr(0, &va) < 0 || 
      argint(1, &pnum) < 0 || 
      argaddr(2, &ua)) {
      return -1;
  }

  // Nếu số lượng trang cần quét lớn hơn PGSIZE*8, trả về -1
  if (pnum > PGSIZE * 8) {
      return -1;
  }

  // Cấp phát một trang làm bộ đệm, kích thước bộ đệm là PGSIZE*8 bit, tức số trang cần quét không được vượt quá giá trị này
  char *buf = kalloc();

  // Khởi tạo bộ đệm, rất quan trọng, vì kalloc() sẽ điền dữ liệu không hợp lệ vào trang được cấp phát thay vì 0
  memset(buf, 0, PGSIZE);

  // Tính vị trí bắt đầu đánh dấu từ bit thứ cnt trong bộ đệm
  int cnt = (pnum / 8 + ((pnum % 8) != 0)) * 8 - pnum;
  printf("cnt = %d\n", cnt);

  // Quét từng trang
  for (int i = 0; i < pnum; i++, cnt++) {
      // Lấy PTE tương ứng với địa chỉ ảo va + i * PGSIZE
      pte_t *p = walk(myproc()->pagetable, va + i * PGSIZE, 0);

      // Nếu PTE có bit PTE_A được bật
      if (*p & PTE_A) {
          // Đánh dấu vị trí tương ứng trong bộ đệm là 1, và xóa bit PTE_A trong PTE
          buf[cnt / 8] |= 1 << (cnt % 8);
          *p &= ~PTE_A;
      }
  }

  // Sao chép kết quả từ kernel sang bộ đệm người dùng
  copyout(myproc()->pagetable, ua, buf, pnum);

  // Giải phóng trang bộ đệm
  kfree(buf);

  return 0;
}
#endif
