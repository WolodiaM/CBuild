local base_url = "";
local function get_vars (meta)
	base_url = pandoc.utils.stringify(meta.BASE_URL or "")end
local function transform_doc_refs(el)
	local symbol = el.target:match("^DOC:(.+)$");
	if symbol then
		el.target = base_url .. "/doc/symbols.html#ID_" .. symbol;
	end
	return el;
end
function Pandoc(doc)
	return  doc:walk({Meta = get_vars}):walk({Link = transform_doc_refs});
end
