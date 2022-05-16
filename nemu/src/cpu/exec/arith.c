#include "cpu/exec.h"

make_EHelper(add) {
	rtl_add(&t2, &id_dest->val, &id_src->val);
    operand_write(id_dest, &t2);

	// 更新ZF,SF标志位
    rtl_update_ZFSF(&t2, id_dest->width);

	// 更新CF标志位
    rtl_sltu(&t0, &t2, &id_dest->val);
    rtl_set_CF(&t0);

	// 更新OF标志位
    rtl_xor(&t0, &id_dest->val, &id_src->val);
    rtl_not(&t0);
    rtl_xor(&t1, &id_dest->val, &t2);
    rtl_and(&t0, &t0, &t1);
    rtl_msb(&t0, &t0, id_dest->width);
    rtl_set_OF(&t0);
	print_asm_template2(add);
}

make_EHelper(sub) {
	rtl_sub(&t3, &id_dest->val, &id_src->val); // 目的操作数减源操作数
	operand_write(id_dest, &t3); //将结果写回目的操作数

	// 更新ZF,SF标志位
	rtl_update_ZFSF(&t3, id_dest->width); 

	// 更新CF标志位
	rtl_sltu(&t1, &id_dest->val, &t3); //当 被减数<减法结果，CF = 1;
	rtl_set_CF(&t1);

	// 更新OF标志位
	rtl_xor(&t1, &id_dest->val, &id_src->val); //当被减数与减数符号不一样时，t1 = 1;
	rtl_xor(&t2, &id_dest->val, &t3);          //当减法结果与被减数符号不一样时，t2 = 1;
	rtl_and(&t0, &t1, &t2);                    //t0判断是否溢出，就是当(负数-正数=正数)和(正数-负数=负数)时为溢出
	rtl_msb(&t0, &t0, id_dest->width);         //取最高位，即符号位
	rtl_set_OF(&t0);
	print_asm_template2(sub);
}

make_EHelper(cmp) {
	rtl_sub(&t3, &id_dest->val, &id_src->val);

	// 更新ZF,SF标志位
	rtl_update_ZFSF(&t3, id_dest->width); // rtl_update_ZFSF函数内部临时变量是t0，所以不能用t0传参，否则更新SF会出错，因为更新ZF时，t0会变
	
	// 更新CF标志位
	rtl_sltu(&t1, &id_dest->val, &t3);
	rtl_set_CF(&t1);

	// 更新OF标志位
	rtl_xor(&t1, &id_dest->val, &id_src->val);
	rtl_xor(&t2, &id_dest->val, &t3);
	rtl_and(&t0, &t1, &t2);
	rtl_msb(&t0, &t0, id_dest->width);
	rtl_set_OF(&t0);

	print_asm_template2(cmp);
}

make_EHelper(inc) {
	rtl_addi(&t3, &id_dest->val, 1);
	operand_write(id_dest, &t3);

	// 更新ZF,SF标志位
	rtl_update_ZFSF(&t3, id_dest->width); // rtl_update_ZFSF函数内部临时变量是t0，所以不能用t0传参，否则更新SF会出错，因为更新ZF时，t0会变

	// 更新OF标志位
	rtl_xor(&t2, &id_dest->val, &t3);
	rtl_msb(&t2, &t2, id_dest->width);
	rtl_set_OF(&t2);
	print_asm_template1(inc);
}

make_EHelper(dec) {
	rtl_subi(&t3, &id_dest->val, 1);
	operand_write(id_dest, &t3);
  
	// 更新ZF,SF标志位
	rtl_update_ZFSF(&t3, id_dest->width); // rtl_update_ZFSF函数内部临时变量是t0，所以不能用t0传参，否则更新SF会出错，因为更新ZF时，t0会变

	// 更新OF标志位
	rtl_xor(&t2, &id_dest->val, &t3);
	rtl_msb(&t2, &t2, id_dest->width);
	rtl_set_OF(&t2);
	print_asm_template1(dec);
}

make_EHelper(neg) {
    if(!id_dest->val)// 操作数为零
        rtl_set_CF(&tzero);
    else{
        t0 = 1;
        rtl_set_CF(&t0);
    }
    t0= -id_dest->val; // 取反
    operand_write(id_dest, &t0);

    // 更新ZF,SF标志位
    rtl_update_ZFSF(&t3, id_dest->width);

    // 更新OF标志位
    rtl_xor(&t1, &id_dest->val, &id_src->val);
    rtl_xor(&t2, &id_dest->val, &t3);
    rtl_and(&t0, &t1, &t2);
    rtl_msb(&t0, &t0, id_dest->width);
    rtl_set_OF(&t0);
    print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &t2, &id_dest->val);
  rtl_get_CF(&t1);
  rtl_add(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &t2, &id_dest->val);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_not(&t0);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  rtl_sub(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &id_dest->val, &t2);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_mul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_imul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_src2->val, &id_src2->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_src2->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_li(&t1, 0);
      rtl_lr_w(&t0, R_AX);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_li(&t1, 0);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_div(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  switch (id_dest->width) {
    case 1:
      rtl_lr_w(&t0, R_AX);
      rtl_sext(&t0, &t0, 2);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_idiv(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(idiv);
}
