-- lazy ------------------------------------------------------------------------

-- install lazy loader if not available
local lazypath = vim.fn.stdpath("data") .. "/lazy/lazy.nvim"
if not vim.loop.fs_stat(lazypath) then
  vim.fn.system({
    "git",
    "clone",
    "--filter=blob:none",
    "https://github.com/folke/lazy.nvim.git",
    "--branch=stable",
    lazypath,
  })
end
vim.opt.rtp:prepend(lazypath)

-- install plugins
require("lazy").setup(
  plugins,
  {
    checker = {
      enabled = true, -- automatically check for updates in the background
    },
    install = {
      colorscheme = { "habamax" },
    },
  }
)
