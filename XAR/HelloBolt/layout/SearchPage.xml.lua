local function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end  -- if
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end  -- for
        return setmetatable(new_table, getmetatable(object))
    end  -- function _copy
    return _copy(object)
end  -- function deepcopy

function OnInitControl(self)
	self:SetObjPos(0,0,"father.width","father.height")
end

function OnFocusChange(self, isFocus)
	local parent = self:GetParent()
	if isFocus then
		--XLMessageBox("focus")
		parent:SetTextureID("texture.edit.light.bkg")
	else
		parent:SetTextureID("")
	end
end

function OnLogoDown(self)
	local owner = self:GetOwnerControl():GetParent()
	owner:GetControlObject("ctrl"):SetObjPos2(0,0,"father.width","father.height")
	
	local resultPage = owner:GetControlObject("ResultPage")
	if resultPage then
		owner:RemoveChild(resultPage)
	else
		return false
	end
	--将各子控件状态设置为初试
	owner:GetControlObject("inputarea"):SetText("")
end
function OnSearchParamKeyWordsChanged(self)
	local control  = self:GetOwnerControl()
	local attr = control:GetAttribute()
	if attr.NewSearchParam == nil then
		attr.NewSearchParam = {KeyWords, Type = 15}
	end
	attr.NewSearchParam.KeyWords = self:GetText()
end
function OnSearchParamTypeChanged(self, eventName, isCheck)--计算搜索参数的值
	local control  = self:GetOwnerControl()
	local attr = control:GetAttribute()
	if attr.NewSearchParam == nil then
		attr.NewSearchParam = {Type = 15}
	end
	local id = self:GetID()
	local offset
	if id == "checkbox.movie" then
		offset = 1
	end
	if id == "checkbox.music" then
		offset = 2
	end
	if id == "checkbox.game" then
		offset = 4
	end
	if id == "checkbox.book" then
		offset = 8
	end
	if isCheck then
		if attr.NewSearchParam.Type == 15 then
			attr.NewSearchParam.Type = offset
		else
			attr.NewSearchParam.Type = attr.NewSearchParam.Type + offset
		end
	else
		attr.NewSearchParam.Type = attr.NewSearchParam.Type - offset
	end
	
	if attr.NewSearchParam.Type == 0 then
		attr.NewSearchParam.Type = 15
	end
	--XLMessageBox(attr.NewSearchParam.Type.." text:"..attr.NewSearchParam.KeyWords)
end 

function OnSearchPageSearchBtnClick(self)--搜索页面的按钮响应
	local ctrl = self:GetOwnerControl()
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local resultPage = objFactory:CreateUIObject("ResultPage", "ResultPage")
	local resultPageEdit = resultPage:GetControlObject("inputarea")
	local listbox = resultPage:GetControlObject("listbox")
	
	local attr = resultPage:GetAttribute()
	local edit = ctrl:GetControlObject("inputarea")
	local movie = ctrl:GetControlObject("checkbox.movie")
	local music = ctrl:GetControlObject("checkbox.music")
	local game = ctrl:GetControlObject("checkbox.game")
	local book = ctrl:GetControlObject("checkbox.book")
	
	local totalCount = resultPage:GetControlObject("totalcount")
	if attr.SearchParam == nil then
		attr.SearchParam = {}
	end
	attr.SearchParam.Type = 0
	if movie:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 1
	end
	if music:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 2
	end
	if game:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 4
	end
	if book:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 8
	end
	if attr.SearchParam.Type == 0 then
		attr.SearchParam.Type = 15
	end
	attr.SearchParam.KeyWords = edit:GetText()
	attr.CurPage = 1
	attr.LastPage = 1
	
	--将搜索页面的UI移出
	ctrl:GetControlObject("ctrl"):SetObjPos2(1920,1080,"father.width","father.height")	
	resultPage:SetObjPos2(0,0,"father.width","father.height")
	resultPageEdit:SetText(attr.SearchParam.KeyWords)
	ctrl:AddChild(resultPage)

	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, 0, 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp) else totalCount:SetText("当前/总数 ".."1-10".."/"..tmp) 
			end 
			if tmp % 10 == 0 then
				attr.LastPage = math.floor(tmp/10)
			else
				attr.LastPage = math.floor(tmp/10) + 1
			end
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end

	listbox:UpdateUI()
end

function OnResultPageSearchBtnClick(self)--结果页面的搜索按钮
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local edit = ctrl:GetControlObject("inputarea")
	local movie = ctrl:GetControlObject("checkbox.movie")
	local music = ctrl:GetControlObject("checkbox.music")
	local game = ctrl:GetControlObject("checkbox.game")
	local book = ctrl:GetControlObject("checkbox.book")
	local listbox = ctrl:GetControlObject("listbox")
	local totalCount = ctrl:GetControlObject("totalcount")
	totalCount:SetText("当前/总数 0/0")
	if attr.SearchParam == nil then
		attr.SearchParam = {}
	end
	attr.SearchParam.Type = 0
	if movie:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 1
	end
	if music:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 2
	end
	if game:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 4
	end
	if book:GetCheck() then
		attr.SearchParam.Type = attr.SearchParam.Type + 8
	end
	if attr.SearchParam.Type == 0 then
		attr.SearchParam.Type = 15
	end
	attr.SearchParam.KeyWords = edit:GetText()
	attr.CurPage = 1
	attr.LastPage = 1
	listbox:ResetContent()
	--更新listbox数据
	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, 0, 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp) else totalCount:SetText("当前/总数 ".."1-10".."/"..tmp) 
			end
			if tmp % 10 == 0 then
				attr.LastPage = math.floor(tmp/10)
			else
				attr.LastPage = math.floor(tmp/10) + 1
			end
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end
	listbox:UpdateUI()
end


function OnFirstPageClick(self)--回到结果首页
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	local totalCount = ctrl:GetControlObject("totalcount")
	
	if attr.CurPage == 1 then
		return
	end
	totalCount:SetText("当前/总数 0/0")
	listbox:ResetContent()
	attr.CurPage = 1
	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, 0, 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp) 
			else 
				totalCount:SetText("当前/总数 ".."1-10".."/"..tmp) 
			end 
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end
	listbox:UpdateUI()
end

function OnPrePageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	local totalCount = ctrl:GetControlObject("totalcount")
	
	if attr.CurPage == 1 then
		return
	end
	totalCount:SetText("当前/总数 0/0")
	listbox:ResetContent()
	attr.CurPage = attr.CurPage - 1
	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, (attr.CurPage-1)*10, (attr.CurPage-1)*10 + 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp) 
			else 
				totalCount:SetText("当前/总数 "..((attr.CurPage*10)-9).."-"..(attr.CurPage*10).."/"..tmp) 
			end 
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end
	listbox:UpdateUI()
end

function OnNextPageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	local totalCount = ctrl:GetControlObject("totalcount")
	
	if attr.CurPage == attr.LastPage then
		return
	end
	
	listbox:ResetContent()
	attr.CurPage = attr.CurPage + 1
	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, (attr.CurPage-1)*10, (attr.CurPage-1)*10 + 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp)
			else 
				totalCount:SetText("当前/总数 "..((attr.CurPage*10)-9).."-"..(attr.CurPage*10).."/"..tmp) 
			end 
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end
	listbox:UpdateUI()
end

function OnLastPageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	local totalCount = ctrl:GetControlObject("totalcount")
	if attr.CurPage == attr.LastPage then
		return
	end
	
	listbox:ResetContent()
	attr.CurPage = attr.LastPage
	
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, (attr.CurPage-1)*10, (attr.CurPage-1)*10 + 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp) 
			else 
				totalCount:SetText("当前/总数 "..((attr.CurPage*10)-9).."-"..tmp.."/"..tmp) 
			end 
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end
	listbox:UpdateUI()
end

function OnSkipPageClick(self)
	local ctrl = self:GetOwnerControl()
	local attr = ctrl:GetAttribute()
	local listbox = ctrl:GetControlObject("listbox")
	local totalCount = ctrl:GetControlObject("totalcount")
	local desPage = ctrl:GetControlObject("skippage.input")
	local desPageNum = tonumber(desPage:GetText())
	
	if desPage:GetText() == "" then
		return
	end
	if desPageNum > attr.LastPage then
		desPageNum = attr.LastPage
	end
	
	listbox:ResetContent()
	attr.CurPage = desPageNum
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	if coolClientProxy then
		coolClientProxy:SearchResource(attr.SearchParam.KeyWords, attr.SearchParam.Type, (attr.CurPage-1)*10, (attr.CurPage-1)*10 + 9, 
		function(t) listbox:AddItem(t) local tmp = t.TotalCount 
			if tmp < 10 then 
				totalCount:SetText("当前/总数 ".."1-"..tmp.."/"..tmp) 
			else 
				if attr.CurPage == attr.LastPage then
					totalCount:SetText("当前/总数 "..((attr.CurPage*10)-9).."-"..tmp.."/"..tmp) 
				else
					totalCount:SetText("当前/总数 "..((attr.CurPage*10)-9).."-"..(attr.CurPage*10).."/"..tmp) 
				end
			end 
		end )	
	else
		totalCount:SetText("测试 当前/总数 1-10/23")
	end
	listbox:UpdateUI()
end

function OnResItemSave(self, eventName, index)
	--XLMessageBox(index)
	local attr = self:GetAttribute()
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	--XLMessageBox(index)
	local path,name,torrent_type,files = coolClientProxy:GetResourceTorrentById(tonumber(attr.ItemDataTable[index].TorrentId), attr.ItemDataTable[index].Name)
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")				
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	local downloadpage = tree:GetUIObject("tabbkg"):GetControlObject("MydownloadPage")
	downloadpage:AddNewDownloadTask(path,name,torrent_type,files)
end

function OnKeyEnterDown(self,eventName)
	if self:GetOwnerControl():GetClass() == "SearchPage" then
		OnSearchPageSearchBtnClick(self:GetOwnerControl():GetControlObject('searchbtn'))
	else
		OnResultPageSearchBtnClick(self:GetOwnerControl():GetControlObject('searchbtn'))
	end
	--XLMessageBox('enterdown')
end