local function include_files_in_codeblock(cb)
	if cb.attributes.file then
		local path = cb.attributes.file
		local file = io.open(path);
		if file == nil then
			pandoc.log.warn("Invlaid file path: '" .. path .. "'");
			return pandoc.CodeBlock("INVALID FILE PROVIDED", cb.attr);
		end
		local content = file:read("*a");
		file:close()
		cb.attributes.file = nil
		return pandoc.CodeBlock(content, cb.attr);
	end
end

return {
	{ CodeBlock = include_files_in_codeblock }
};
