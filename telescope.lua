require("telescope").setup({
  pickers = {
    find_files = {
      theme = "ivy",
    },
    live_grep = {
      theme = "ivy",
    },
  },
})

local tele_builtin = require("telescope.builtin")
-- vim.keymap.set({"v", "n"}, "<leader>fs", tele_builtin.grep_string, {})
vim.keymap.set({"v", "n"}, "<leader>fs", function() tele_builtin.grep_string({cwd = "~"}) end, {})
vim.keymap.set("n", "<leader>fa", tele_builtin.find_files, {})
vim.keymap.set("n", "<leader>ff", tele_builtin.git_files, {})
vim.keymap.set("n", "<leader>fg", tele_builtin.live_grep, {})
vim.keymap.set("n", "<leader>fb", tele_builtin.buffers, {})
vim.keymap.set("n", "<leader>fh", tele_builtin.help_tags, {})

