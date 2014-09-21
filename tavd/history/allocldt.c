  int sel = __dpmi_allocate_ldt_descriptors (1);

  if (sel)
    return false;

  __dpmi_set_descriptor_access_rights (sel, 0x8092);
  dword limit = 0x1000 - 1;
  __dpmi_meminfo info;

  info.size = limit;
  info.address = 0; // physical adress;
  __dpmi_physical_address_mapping (&info);
  __dpmi_set_segment_base_address (sel, info.address);
  __dpmi_set_segment_limit (sel, limit);

  __dpmi_free_physical_address_mapping (&info);
  __dpmi_free_ldt_descriptor (sel);
