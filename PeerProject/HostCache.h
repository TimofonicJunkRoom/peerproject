//
// HostCache.h
//
// This file is part of PeerProject (peerproject.org) � 2008-2012
// Portions copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU Affero General Public License
// as published by the Free Software Foundation (fsf.org);
// either version 3 of the License, or later version at your option.
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Affero General Public License 3.0 (AGPLv3) for details:
// (http://www.gnu.org/licenses/agpl.html)
//

#pragma once

class CNeighbour;
class CG1Packet;
class CVendor;
class CHostCacheHost;
class CHostCacheList;
class CHostCache;


class CHostCacheHost
{
public:
	CHostCacheHost(PROTOCOLID nProtocol);

	// Attributes: Host Information
	PROTOCOLID	m_nProtocol;		// Host protocol (PROTOCOL_*)
	CString		m_sAddress;			// Host full address (unresolved)
	IN_ADDR		m_pAddress; 		// Host IP address
	WORD		m_nPort;			// Host TCP port number
	WORD		m_nUDPPort; 		// Host UDP port number
	CVendor*	m_pVendor;			// Vendor handler from VendorCache
	BOOL		m_bPriority;		// Host cannot be removed on failure
	DWORD		m_nUserCount;		// G2 leaf count / ED2K/DC user count
	DWORD		m_nUserLimit;		// G2 leaf limit / ED2K/DC user limit
	DWORD		m_nFileLimit;		// ED2K-server file limit
	DWORD		m_nTCPFlags;		// ED2K TCP flags (ED2K_SERVER_TCP_*)
	DWORD		m_nUDPFlags;		// ED2K UDP flags (ED2K_SERVER_UDP_*)
	BOOL		m_bCheckedLocally;	// Host was successfully accessed via TCP or UDP
	CString		m_sName;			// Host name
	CString		m_sDescription;		// Host description
	CString		m_sUser;			// User name on this server (DC)
	CString		m_sPass;			// User password on this server (DC)
	CString		m_sCountry; 		// Country code

	// Attributes: Contact Times
	DWORD		m_tAdded;			// Time when host was constructed (in ticks)
	DWORD		m_tRetryAfter;		// G2 retry time according G2_PACKET_RETRY_AFTER packet (in seconds)
	DWORD		m_tConnect; 		// TCP connect time (in seconds)
	DWORD		m_tQuery;			// G2 / ED2K query time (in seconds)
	DWORD		m_tAck; 			// Time when we sent something requires acknowledgment (0 - not required)
	DWORD		m_tStats;			// ED2K stats UDP request
	DWORD		m_tFailure; 		// Last failure time
	DWORD		m_nFailures;		// Failures counter
	DWORD		m_nDailyUptime;		// Daily uptime (G1)

	// Attributes: Query Keys
	DWORD		m_tKeyTime; 		// G2 time when query key was received
	DWORD		m_nKeyValue;		// G2 query key
	DWORD		m_nKeyHost; 		// G2 query key host

	// Attributes: DHT
	BOOL			m_bDHT; 		// Host is DHT capable
	Hashes::BtGuid	m_oBtGUID;		// Host GUID (160 bit)
	CArray< BYTE >	m_Token;		// Host access token

	// Attributes: Kademlia
	Hashes::Guid	m_oGUID;		// Host GUID (128 bit)
	BYTE			m_nKADVersion;	// Kademlia version

	bool		ConnectTo(BOOL bAutomatic = FALSE);
	CString		ToString(bool bLong = true) const;		// "10.0.0.1:6346 2002-04-30T08:30Z"
	bool		IsExpired(const DWORD tNow) const;		// Is this host expired?
	bool		IsThrottled(const DWORD tNow) const;	// Is host temporary throttled down?
	bool		CanConnect(const DWORD tNow) const;		// Can we connect to this host now?
	bool		CanQuote(const DWORD tNow) const;		// Is this a recently seen host?
	bool		CanQuery(const DWORD tNow) const;		// Can we UDP query this host? (G2/ed2k)
	void		SetKey(const DWORD nKey, const IN_ADDR* pHost = NULL);

	DWORD		Seen() const;		// Get host last seen time
	CString		Address() const;	// Get host address as string

protected:
	DWORD		m_tSeen;			// Host last seen time

	// Return: true - if tSeen changed, false - otherwise.
	bool		Update(WORD nPort, DWORD tSeen = 0, LPCTSTR pszVendor = NULL, DWORD nUptime = 0, DWORD nCurrentLeaves = 0, DWORD nLeafLimit = 0);
	void		Serialize(CArchive& ar, int nVersion);

	friend class CHostCacheList;

private:
	CHostCacheHost(const CHostCacheHost&);
	CHostCacheHost& operator=(const CHostCacheHost&);
};

typedef CHostCacheHost* CHostCacheHostPtr;

template<>
struct std::less< IN_ADDR > : public std::binary_function< IN_ADDR, IN_ADDR, bool>
{
	inline bool operator()(const IN_ADDR& _Left, const IN_ADDR& _Right) const throw()
	{
		return ( ntohl( _Left.s_addr ) < ntohl( _Right.s_addr ) );
	}
};

typedef std::multimap< IN_ADDR, CHostCacheHostPtr > CHostCacheMap;
typedef std::pair< IN_ADDR, CHostCacheHostPtr > CHostCacheMapPair;
typedef CHostCacheMap::iterator CHostCacheMapItr;

template<>
struct std::less< CHostCacheHostPtr > : public std::binary_function< CHostCacheHostPtr, CHostCacheHostPtr, bool>
{
	inline bool operator()(const CHostCacheHostPtr& _Left, const CHostCacheHostPtr& _Right) const throw()
	{
		return ( _Left->Seen() > _Right->Seen() );
	}
};

typedef std::multiset< CHostCacheHostPtr > CHostCacheIndex;
typedef std::pair < CHostCacheIndex::iterator, CHostCacheIndex::iterator > CHostCacheTimeItPair;
typedef CHostCacheIndex::const_iterator CHostCacheIterator;
typedef CHostCacheIndex::const_reverse_iterator CHostCacheRIterator;

struct good_host : public std::binary_function< CHostCacheMapPair, BOOL, bool>
{
	inline bool operator()(const CHostCacheMapPair& _Pair, const BOOL& _bLocally) const throw()
	{
		return ( _Pair.second->m_nFailures == 0 &&
			( _Pair.second->m_bCheckedLocally || _bLocally ) );
	}
};

struct is_host : public std::binary_function< CHostCacheMapPair, CHostCacheHostPtr, bool>
{
	inline bool operator()(const CHostCacheMapPair& _Pair, const CHostCacheHostPtr& _bLocally) const throw()
	{
		return ( _Pair.second == _bLocally );
	}
};

struct is_address : public std::binary_function< CHostCacheMapPair, LPCTSTR, bool>
{
	inline bool operator()(const CHostCacheMapPair& _Pair, const LPCTSTR& _bLocally) const throw()
	{
		return ( _Pair.second->m_sAddress.CompareNoCase( _bLocally ) == 0 );
	}
};

class CHostCacheList
{
public:
	CHostCacheList(PROTOCOLID nProtocol);
	virtual ~CHostCacheList();

	PROTOCOLID			m_nProtocol;
	DWORD				m_nCookie;
	mutable CMutex		m_pSection;

	CHostCacheHostPtr	Add(const IN_ADDR* pAddress, WORD nPort, DWORD tSeen = 0, LPCTSTR pszVendor = NULL, DWORD nUptime = 0, DWORD nCurrentLeaves = 0, DWORD nLeafLimit = 0, LPCTSTR szAddress = NULL);
	BOOL				Add(LPCTSTR pszHost, DWORD tSeen = 0, LPCTSTR pszVendor = NULL, DWORD nUptime = 0, DWORD nCurrentLeaves = 0, DWORD nLeafLimit = 0); 	// Add host in form "IP:Port SeenTime"
	void				Update(CHostCacheHostPtr pHost, WORD nPort = 0, DWORD tSeen = 0, LPCTSTR pszVendor = NULL, DWORD nUptime = 0, DWORD nCurrentLeaves = 0, DWORD nLeafLimit = 0);
	CHostCacheMapItr	Remove(CHostCacheHostPtr pHost);
	CHostCacheMapItr	Remove(const IN_ADDR* pAddress);
	void				OnResolve(LPCTSTR szAddress, const IN_ADDR* pAddress, WORD nPort);
	void				OnFailure(LPCTSTR szAddress, bool bRemove = true);
	void				OnFailure(const IN_ADDR* pAddress, WORD nPort, bool bRemove = true);
	void				OnSuccess(const IN_ADDR* pAddress, WORD nPort, bool bUpdate = true);
	void				PruneOldHosts(DWORD tNow);
	void				SanityCheck();
	void				Clear();
	void				Serialize(CArchive& ar, int nVersion);

	inline CHostCacheIterator Begin() const throw()
	{
		return m_HostsTime.begin();
	}

	inline CHostCacheIterator End() const throw()
	{
		return m_HostsTime.end();
	}

	inline CHostCacheRIterator RBegin() const throw()
	{
		return m_HostsTime.rbegin();
	}

	inline CHostCacheRIterator REnd() const throw()
	{
		return m_HostsTime.rend();
	}

	inline CHostCacheHostPtr GetNewest() const throw()
	{
		return IsEmpty() ? NULL : *Begin();
	}

	inline CHostCacheHostPtr GetOldest() const throw()
	{
		return IsEmpty() ? NULL : *( End()-- );
	}

	inline bool IsEmpty() const throw()
	{
		return m_HostsTime.empty();
	}

	inline DWORD GetCount() const throw()
	{
		return (DWORD)m_Hosts.size();
	}

	inline CHostCacheHostPtr Find(const IN_ADDR* pAddress) const throw()
	{
		if ( pAddress->s_addr == INADDR_ANY ||
			 pAddress->s_addr == INADDR_NONE )
			return NULL;
		CQuickLock oLock( m_pSection );
		CHostCacheMap::const_iterator i = m_Hosts.find( *pAddress );
		return ( i != m_Hosts.end() ) ? (*i).second : NULL;
	}

	inline CHostCacheHostPtr Find(LPCTSTR szAddress) const throw()
	{
		if ( ! szAddress || ! *szAddress )
			return NULL;
		CQuickLock oLock( m_pSection );
		CHostCacheMap::const_iterator i =
			std::find_if( m_Hosts.begin(), m_Hosts.end(),
			std::bind2nd( is_address(), szAddress ) );
		return ( i != m_Hosts.end() ) ? (*i).second : NULL;
	}

	inline bool Check(const CHostCacheHostPtr pHost) const throw()
	{
		CQuickLock oLock( m_pSection );
		return std::find( m_HostsTime.begin(), m_HostsTime.end(), pHost ) != m_HostsTime.end();
	}

	inline DWORD CountHosts(const BOOL bCountUncheckedLocally = FALSE) const throw()
	{
		CQuickLock oLock( m_pSection );
		return (DWORD)(size_t) std::count_if( m_Hosts.begin(), m_Hosts.end(),
			std::bind2nd( good_host(), bCountUncheckedLocally ) );
	}

	inline CHostCacheHostPtr GetForDHTQuery() const throw()
	{
		const DWORD tNow = static_cast< DWORD >( time( NULL ) );
		for ( CHostCacheIterator it = m_HostsTime.begin() ; it != m_HostsTime.end() ; ++it )
		{
			CHostCacheHostPtr pHost = (*it);
			if ( pHost->CanQuery( tNow ) && pHost->m_bDHT && pHost->m_oBtGUID )
				return pHost;
		}
		return NULL;
	}

	inline CHostCacheHostPtr GetOldestForQuery() const throw()
	{
		const DWORD tNow = static_cast< DWORD >( time( NULL ) );
		for ( CHostCacheRIterator it = m_HostsTime.rbegin() ; it != m_HostsTime.rend() ; ++it )
		{
			CHostCacheHostPtr pHost = (*it);
			if ( pHost->CanQuery( tNow ) )
				return pHost;
		}
		return NULL;
	}

protected:
	CHostCacheMap		m_Hosts;		// Hosts map (sorted by IP)
	CHostCacheIndex		m_HostsTime;	// Host index (sorted from newer to older)

	CHostCacheHostPtr	AddInternal(const IN_ADDR* pAddress, WORD nPort, DWORD tSeen, LPCTSTR pszVendor, DWORD nUptime, DWORD nCurrentLeaves, DWORD nLeafLimit, LPCTSTR szAddress);
	void				PruneHosts();
};


class CHostCache
{
public:
	CHostCache();

	CHostCacheList		Gnutella2;
	CHostCacheList		Gnutella1;
	CHostCacheList		G1DNA;
	CHostCacheList		eDonkey;
	CHostCacheList		DC;
	CHostCacheList		Kademlia;
	CHostCacheList		BitTorrent;

	BOOL				Load();
	BOOL				Save();

	// Import various host files, return imported host count
	int					Import(LPCTSTR pszFile, BOOL bFreshOnly = FALSE);
	int					ImportHubList(CFile* pFile);	// Import DC++ hub list .xml.bz2 file
	int					ImportMET(CFile* pFile);		// Import eDonkey2000 servers .met file
	int					ImportNodes(CFile* pFile);		// Import Kademlia nodes .dat file
	//int				ImportCache(CFile* pFile);		// ToDo: Support custom G2/Gnutella import/export .xml/.dat

	bool				CheckMinimumED2KServers();
	BOOL				Check(const CHostCacheHostPtr pHost) const;
	CHostCacheHostPtr	Find(const IN_ADDR* pAddress) const;
	CHostCacheHostPtr	Find(LPCTSTR szAddress) const;
	void				Remove(CHostCacheHostPtr pHost);
	void				PruneOldHosts();
	void				SanityCheck();
	void				OnFailure(const IN_ADDR* pAddress, WORD nPort, PROTOCOLID nProtocol = PROTOCOL_NULL, bool bRemove = true);
	void				OnSuccess(const IN_ADDR* pAddress, WORD nPort, PROTOCOLID nProtocol = PROTOCOL_NULL, bool bUpdate = true);

	inline bool EnoughED2KServers() const throw()
	{
		return ( eDonkey.CountHosts( TRUE ) >= 6 );
	}

	inline CHostCacheList* ForProtocol(PROTOCOLID nProtocol) throw()
	{
		switch ( nProtocol )
		{
		case PROTOCOL_G1:
			return &Gnutella1;
		case PROTOCOL_G2:
			return &Gnutella2;
		case PROTOCOL_ED2K:
			return &eDonkey;
		case PROTOCOL_BT:
			return &BitTorrent;
		case PROTOCOL_KAD:
			return &Kademlia;
		case PROTOCOL_DC:
			return &DC;
		default:
		//	ASSERT(FALSE);
			return NULL;
		}
	}

protected:
	CList< CHostCacheList* >	m_pList;
	mutable CCriticalSection	m_pSection;
	DWORD		m_tLastPruneTime;

	void		Serialize(CArchive& ar);
	void		Clear();
	int			LoadDefaultED2KServers();
};

extern CHostCache HostCache;
