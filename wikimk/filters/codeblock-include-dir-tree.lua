--- @class FSObject One filesystem object
--- @field name string Filename
--- @field children FSObject[] Children
--- @field is_dir boolean Type of this entry

local function gen_dir_list(path)
	local ret = {}; --- @type FSObject[]
	local files = pandoc.system.list_directory(path);
	for _, file in ipairs(files) do
		local p = pandoc.path.join({path, file});
		-- Add directory children if applicable
		local children = {}; --- @type FSObject[]
		local is_dir = false;
		-- if pandoc.path.exists(p, "directory") then -- pandoc 3.7.1
		if pcall(pandoc.system.list_directory, p) then
			children = gen_dir_list(p);
			is_dir = true;
		end
		table.insert(ret, {name = file, children = children, is_dir = is_dir});
	end
	table.sort(ret, function(a, b)
		if a.is_dir and not b.is_dir then return true; end
		if not a.is_dir and b.is_dir then return false; end
		return a.name:lower() < b.name:lower()
	end);
	return ret;
end
local function print_dir_list(tree, prefix)
	local ret = "";
	for i, file in ipairs(tree) do
		ret = ret .. prefix .. (i == #tree and "└── " or "├── ") .. file.name .. "\n";
		if file.is_dir then
			local new_prefix = prefix .. (i == #tree and "    " or "│   ")
			ret = ret .. print_dir_list(file.children, new_prefix);
		end
	end
	return ret;
end
local function include_dir_tree_in_codeblock(cb)
	if cb.attributes.dir then
		local tree = gen_dir_list(cb.attributes.dir);
		local content = cb.attributes.dir .. "\n";
		content = content .. print_dir_list(tree, "");
		cb.attributes.dir = nil
		return pandoc.CodeBlock(content, cb.attr, true);
	end
end

return {
	{ CodeBlock = include_dir_tree_in_codeblock }
};
