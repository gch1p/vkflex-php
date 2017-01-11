PHP_ARG_ENABLE(vkflex, whether to enable vkflex extension
[ --enable-vkflex Enable vkflex extension])
 
if test "$PHP_VKFLEX" = "yes"; then
  AC_DEFINE(HAVE_VKFLEX, 1, [Whether you have vkflex extension])
  PHP_NEW_EXTENSION(vkflex, vkflex.c vkext_flex.c, $ext_shared)
fi
