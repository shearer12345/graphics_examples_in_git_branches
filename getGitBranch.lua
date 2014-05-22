-- execute a shell command and capture the output, using io.popen
local branchFile = io.popen("git symbolic-ref -q --short HEAD") -- store the output in a "file"

--get the file's contents
branch = branchFile:read("*a") 

-- remove line breaks
branch = string.gsub(branch , "\n", "") 

-- if empty name it HEAD
if branch == "" then
  branch = "HEAD"
end

return branch