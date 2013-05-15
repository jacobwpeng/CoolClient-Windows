local function SortBySize(userData,order)
	if order == "down" then
		sortFunc = function(a, b) return b.Size < a.Size end
	else
		sortFunc = function(a, b) return a.Size < b.Size end
	end
	table.sort(userData,sortFunc)
end
local function SortByProgress(userData,order)
	if order == "down" then
		sortFunc = function(a, b) return b.Progress < a.Progress end
	else
		sortFunc = function(a, b) return a.Progress < b.Progress end
	end
	table.sort(userData,sortFunc)
end
local function SortByDownload(userData,order)
	if order == "down" then
		sortFunc = function(a, b) return b.Download < a.Download end
	else
		sortFunc = function(a, b) return a.Download < b.Download end
	end
	table.sort(userData,sortFunc)
end
function Sort(self, jobTable)
	local attr = self:GetAttribute()
	if attr.SortBy == 'size' then
		SortBySize(jobTable,attr.Order)
	elseif attr.SortBy == 'progress' then
		SortByProgress(jobTable,attr.Order)
	elseif attr.SortBy == 'download' then
		SortByDownload(jobTable,attr.Order)
	end
end
function OnSizeClick(self)
	local statusBar = self:GetFather():GetOwnerControl()
	local listbox = statusBar:GetOwnerControl():GetControlObject("listbox")
	local listboxAttr = listbox:GetAttribute()
	local attr = statusBar:GetAttribute()
	if attr.SortBy ~= "size" then
		attr.SortBy = "size"
		attr.Order = "down"
	else
		if attr.Order == "down" then
			attr.Order = "up"
		else
			attr.Order = "down"
		end
	end
	if attr.Order == "down" then
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
	if attr.SortBy ~= "progress" then
		attr.SortBy = "progress"
		attr.Order = "down"
	else
		if attr.Order == "down" then
			attr.Order = "up"
		else
			attr.Order = "down"
		end
	end
	if attr.Order == "down" then
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
	if attr.SortBy ~= "download" then
		attr.SortBy = "download"
		attr.Order = "down"
	else
		if attr.Order == "down" then
			attr.Order = "up"
		else
			attr.Order = "down"
		end
	end
	if attr.Order == "down" then
		sortFunc = function(a, b) return b.Download < a.Download end
	else
		sortFunc = function(a, b) return a.Download < b.Download end
	end
	
	table.sort(listboxAttr.ItemDataTable,sortFunc)
	listbox:UpdateUI()
end