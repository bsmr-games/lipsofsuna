--- Account manager.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.account.account_manager
-- @alias AccountManager

local Account = require("core/account/account")
local Class = require("system/class") 
local Hooks = require("system/hooks")
local Password = require("system/password")
local Serializer = require("system/serializer")

--- Account manager.
-- @type AccountManager
local AccountManager = Class("AccountManager")

--- Creates a new account database.
-- @param clss AccountManager class.
-- @param db Database.
-- @return AccountManager.
AccountManager.new = function(clss)
	local self = Class.new(clss)
	self.login_hooks = Hooks()
	self.serializer = Serializer{}
	self.__accounts_by_client = {}
	self.__accounts_by_login = setmetatable({}, {__mode = "v"})
	self.__version = "2"
	self.__salt = Password:random_salt()
	return self
end

--- Checks if the password matches that of the account.
-- @param self AccountManager.
-- @param account Account.
-- @param password Password.
-- @return True if matched. False otherwise.
AccountManager.check_password = function(self, account, password)
	local hash = self:hash_password(account.login, password)
	return hash == account.password
end

--- Creates an empty account.
-- @param self AccountManager.
-- @param client Client.
-- @param login Login name.
-- @param password Password.
-- @return Account.
AccountManager.create_account = function(self, client, login, password)
	local hash = self:hash_password(login, password)
	local account = Account(client, login, hash)
	self.__accounts_by_client[client] = account
	self.__accounts_by_login[login] = account
	self:save_account(account)
	return account
end

--- Creates a hashed version of the password.
-- @param self AccountManager.
-- @param login Login name.
-- @param password Password string.
-- @return Hashed password string.
AccountManager.hash_password = function(self, login, password)
	return Password:hash(password, self.__salt)
end

--- Loads or creates an account.
-- @param self AccountManager.
-- @param client Client.
-- @param login Login name.
-- @param password Password.
-- @return Account or nil, status message or nil.
AccountManager.load_account = function(self, client, login, password)
	local hash = self:hash_password(login, password)
	local account = Account(client, login, hash)
	self.__accounts_by_client[client] = account
	self.__accounts_by_login[login] = account
	return account, "login successful"
end

--- Logs out a client.
-- @param self AccountManager.
-- @param client Client.
-- @param object Object to save as a character. Nil to save no character.
AccountManager.logout_client = function(self, client, object)
	-- Find the account.
	local account = self.__accounts_by_client[client]
	if not account then return end
	-- Save the account if the character was removed.
	if not object then self:save_account(account) end
	-- Remove the account.
	account.client = nil
	self.__accounts_by_client[client] = nil
	self.__accounts_by_login[account.login] = nil
end

--- Resets the account database.
-- @param self AccountManager.
AccountManager.reset = function(self)
	-- Initialize password hashing.
	self.__salt = Password:random_salt()
	self:set_option("password_salt", self.__salt)
	-- Set the version number.
	self:set_option("account_version", self.__version)
end

--- Strips character data from all accounts.
-- @param self AccountDatabase.
AccountManager.reset_characters = function(self)
end

--- Saves a player account.
-- @param self AccountManager.
-- @param account Account.
-- @param object Player object, or nil.
AccountManager.save_account = function(self, account, object)
end

--- Saves all active player accounts.
-- @param self AccountManager.
AccountManager.save_accounts = function(self)
	for k,v in pairs(Server.players_by_client) do
		self:save_account(v.account, v)
	end
end

--- Gets an active account by client.
-- @param self AccountManager.
-- @param value Client.
-- @return Account if found. Nil otherwise.
AccountManager.get_account_by_client = function(self, value)
	return self.__accounts_by_client[value]
end

--- Gets an active account by client.
-- @param self AccountManager.
-- @param value Login name.
-- @return Account if found. Nil otherwise.
AccountManager.get_account_by_login = function(self, value)
	return self.__accounts_by_login[value]
end

--- Gets an account option.
-- @param self AccountManager.
-- @param key Option name.
-- @return Option value.
AccountManager.get_option = function(self, key)
	return self.__options[key]
end

--- Sets an account database option.
-- @param self AccountManager.
-- @param key Option name.
-- @param value Option value.
AccountManager.set_option = function(self, key, value)
	self.__options[key] = value
end

return AccountManager
