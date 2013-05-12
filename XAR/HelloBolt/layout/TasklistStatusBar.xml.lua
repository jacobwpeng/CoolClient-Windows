function OnSizeClick(self)
	local statusBar = self:GetFather():GetOwnerControl()
	local listbox = statusBar:GetOwnerControl():GetControlObject("listbox")
	local listboxAttr = listbox:GetAttribute()
	local attr = statusBar:GetAttribute()
	local btnAttr = self:GetAttribute()
	if attr.SortBy ~= "size" then
		attr.SortBy = "size"
		btnAttr.Order = "down"
	else
		if btnAttr.Order == "down" then
			btnAttr.Order = "up"
		else
			btnAttr.Order = "down"
		end
	end
	if btnAttr.Order == "down" then
		sortFunc = function(a, b) return b.Size < a.Size end
	else
		sortFunc = function(a, b) return a.Size < b.Size end
	end
	
	table.sort(listboxAttr.ItemDataTable,sortFunc)
end