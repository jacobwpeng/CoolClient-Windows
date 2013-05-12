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
	listbox:UpdateUI()
end

function OnProgressClick(self)
	local statusBar = self:GetFather():GetOwnerControl()
	local listbox = statusBar:GetOwnerControl():GetControlObject("listbox")
	local listboxAttr = listbox:GetAttribute()
	local attr = statusBar:GetAttribute()
	local btnAttr = self:GetAttribute()
	if attr.SortBy ~= "progress" then
		attr.SortBy = "progress"
		btnAttr.Order = "down"
	else
		if btnAttr.Order == "down" then
			btnAttr.Order = "up"
		else
			btnAttr.Order = "down"
		end
	end
	if btnAttr.Order == "down" then
		sortFunc = function(a, b) return b.Progress < a.Progress end
	else
		sortFunc = function(a, b) return a.Progress < b.Progress end
	end
	
	table.sort(listboxAttr.ItemDataTable,sortFunc)
	listbox:UpdateUI()
end

function OnDownloadClick(self)
	local statusBar = self:GetFather():GetOwnerControl()
	local listbox = statusBar:GetOwnerControl():GetControlObject("listbox")
	local listboxAttr = listbox:GetAttribute()
	local attr = statusBar:GetAttribute()
	local btnAttr = self:GetAttribute()
	if attr.SortBy ~= "download" then
		attr.SortBy = "download"
		btnAttr.Order = "down"
	else
		if btnAttr.Order == "down" then
			btnAttr.Order = "up"
		else
			btnAttr.Order = "down"
		end
	end
	if btnAttr.Order == "down" then
		sortFunc = function(a, b) return b.Download < a.Download end
	else
		sortFunc = function(a, b) return a.Download < b.Download end
	end
	
	table.sort(listboxAttr.ItemDataTable,sortFunc)
	listbox:UpdateUI()
end